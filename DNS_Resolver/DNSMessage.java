import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;

/**
 * Driver class of MSDNS where reading and writing of DNS packets takes place.
 */
public class DNSMessage {
    private DNSHeader header_;
    private DNSQuestion[] questions_;
    private DNSRecord[] answers_;
    private DNSRecord[] authorityRecords_;
    private DNSRecord[] additionalRecords_;
    private byte[] message_;
    public DNSHeader getHeader_() {return header_;}
    public DNSQuestion[] getQuestions_() {
        return questions_;
    }
    public DNSRecord getAnswer() { return answers_[0]; }

    DNSMessage(){
        header_ = new DNSHeader();
    }

    /**
     * Read data passed in by a Datagram Packet and construct DNSMessage using decoders from DNSHeader, DNSQuestion
     * and DNSRecord
     * @param bytes passed from Datagram Packet as either a request from a client or a response from Google's DNS server
     * @return a new DNSMessage containing decoded data from incoming data packet
     */
    static DNSMessage decodeMessage(byte[] bytes) throws IOException {
        ByteArrayInputStream inputStream = new ByteArrayInputStream(bytes);
        DNSMessage message = new DNSMessage();
        message.message_ = bytes; // Save the message for handling compression of domain names. Will need to re-read so need to store whole packet
        message.header_ = DNSHeader.decodeHeader(inputStream);
        message.questions_ = new DNSQuestion[message.header_.getQDCOUNT_()];
        message.answers_ = new DNSRecord[message.header_.getANCOUNT_()];
        message.authorityRecords_ = new DNSRecord[message.header_.getNSCOUNT_()];
        message.additionalRecords_ = new DNSRecord[message.header_.getARCOUNT_()];
        for (int i = 0; i < message.header_.getQDCOUNT_(); i++){
            message.questions_[i] = (DNSQuestion.decodeQuestion(inputStream, message));
        }
        for (int i = 0; i < message.header_.getANCOUNT_(); i++){
            message.answers_[i] = DNSRecord.decodeRecord(inputStream, message);
        }
        for (int i = 0; i < message.header_.getNSCOUNT_(); i++){
            message.authorityRecords_[i] = DNSRecord.decodeRecord(inputStream, message);
        }
        for (int i = 0; i < message.header_.getARCOUNT_(); i++){
            message.additionalRecords_[i] = DNSRecord.decodeRecord(inputStream, message);
        }
        return message;
    }

    /**
     * Read domain name from inputStream wherever present and handle compressed domain names by reading previously
     * read domain names when encountered.
     * @param inputStream what we will be reading domain names from.
     * @return a String[] for convenience when we need to rewrite the domain name in the future. Converting from a
     * String[] to a String would add complications with splitting this string back to a String[]
     */
    String[] readDomainName(InputStream inputStream) {
        ArrayList<String> domainParts = new ArrayList<>();
        try {
            int expectedLength = inputStream.readNBytes(1)[0]; // Find length of first piece of domain name
            while (expectedLength != 0) { // Domain name finishes with 0
                // if compressed...
                if ((expectedLength & 0xC0) == 0xC0){ // The first 2 bits of compress domain names are '1''1' or 0xC0
                    byte[] pointerByte = inputStream.readNBytes(1);
                    int pointer = ((((byte)expectedLength & 0x3F) << 8) | (pointerByte[0])); // length is 6 bits
                    Collections.addAll(domainParts, readDomainName(pointer)); // Don't want to store compressed address
                    // so read previous notations of domain name
                    break; // exit loop once domain name is written
                }
                // if not compressed...
                byte[] domainByte = inputStream.readNBytes(expectedLength);
                String domainString = new String(domainByte, StandardCharsets.UTF_8);
                expectedLength = inputStream.readNBytes(1)[0];
                domainParts.add(domainString);
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        return domainParts.toArray(new String[0]);
    }

    /**
     * Used to create a new inputStream for use by readDomainName with start and length of domain name to read
     * @param firstByte where to start reading from
     * @return a String[] containing parts of the domainName to read.
     */
    String[] readDomainName(int firstByte) throws IOException {
        ByteArrayInputStream inputStream = new ByteArrayInputStream(this.message_, firstByte, this.message_.length);
        return readDomainName(inputStream);
    }

    /**
     * Creates a DNSMessage to be converted to bytes to be sent to client
     * @param request from client to be passed to build header and used to pull questions to reply to client
     * @param answers will be pulled from cache to send to client
     * @return a new DNSMessage containing everything needed to respond to client
     */
    static DNSMessage buildResponse(DNSMessage request, DNSRecord[] answers){
        DNSMessage response = new DNSMessage();
        response.header_ = DNSHeader.buildHeaderForResponse(request, response);
        response.questions_ = request.questions_;
        response.answers_ = answers;
        return response;
    }

    /**
     * Converts a DNSMessage to bytes to be sent to client
     * @param question is used to pull specific answers for questions looped in DNSServer. Send a response for
     *                 every question
     * @return a byte[] containing all the response to client
     */
    byte[] toBytes(DNSQuestion question) throws IOException {
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        HashMap<String, Integer> domainNameLocation = new HashMap<>();
        DNSRecord[] answers = new DNSRecord[2];
        answers[0] = DNSCache.dnsCache.get(question);
        answers[1] = this.additionalRecords_[0];
        DNSMessage message = DNSMessage.buildResponse(this, answers);
        message.header_.writeBytes(outputStream);
        for (DNSQuestion q : message.questions_) {
            q.writeBytes(outputStream, domainNameLocation);
        }
        for (DNSRecord r : message.answers_) {
            r.writeBytes(outputStream, domainNameLocation);
        }
        return outputStream.toByteArray();
    }

    /**
     * Write domain name where needed. First time written will be complete domain name and every subsequent write will
     * be compressed.
     * @param outputStream where domain name will be written
     * @param domainLocations HashMap storing an int value that will be written in the compressed domain name specifying
     *                        where domain name is written
     * @param domainPieces used to pull domain location from domainLocations
     */
    static void writeDomainName(ByteArrayOutputStream outputStream, HashMap<String,Integer> domainLocations, String[] domainPieces) throws IOException {
        DNSMessage message = new DNSMessage();
        String domainName = message.joinDomainName(domainPieces);
        if (domainLocations.containsKey(domainName)){ // If domain name has already been written, write compressed
            byte[] domainNameByte = new byte[2];
            int data = domainLocations.get(domainName);
            domainNameByte[0] = (byte)(((data >> 8 ) & 0xFF) | 0xC0); // OR with 0xC0 to keep compressed domain name flag
            domainNameByte[1] = (byte)(data & 0xFF);
            outputStream.write(domainNameByte);
        }
        else{ // Not yet written, so write full (uncompressed) domain name
            int domainLocation = outputStream.size();
            for (String domainPiece : domainPieces){
                byte pieceLength = (byte)(domainPiece.length());
                byte[] piece = domainPiece.getBytes();
                outputStream.write(pieceLength);
                outputStream.write(piece);
            }
            outputStream.write((byte)0); // Need to write 0 to mark zero length specifying end of domain name
            domainLocations.put(domainName, domainLocation); // add location to HashMap
        }
    }

    /**
     * Used for pulling domain locations from domainLocation HashMap for use in writing compressed domain names
     * @param pieces String pieces to combine into single string
     * @return a single String representing a complete domain name
     */
    String joinDomainName(String[] pieces){
        return String.join(".", pieces);
    }

    @Override
    public String toString() {
        return "DNSMessage{" +
                "header_=" + header_ +
                ", questions_=" + Arrays.toString(questions_) +
                ", answers_=" + Arrays.toString(answers_) +
                '}';
    }

    /**
     * Used for looping in DNSServer. Differentiates responses from Google and requests from clients
     * @param message is what we decode
     * @return true if message is query and false otherwise
     */
    public static boolean isQuery(DNSMessage message){
        return message.getHeader_().getQR_() == 0;
    }
}
