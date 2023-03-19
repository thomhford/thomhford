import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Objects;

/**
 * Question section format
 *
 * The question section is used to carry the "question" in most queries,
 * i.e., the parameters that define what is being asked.  The section
 * contains QDCOUNT (usually 1) entries, each of the following format:
 *
 *                                     1  1  1  1  1  1
 *       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                                               |
 *     /                     QNAME                     /
 *     /                                               /
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                     QTYPE                     |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                     QCLASS                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * where:
 *
 * QNAME           a domain name represented as a sequence of labels, where
 *                 each label consists of a length octet followed by that
 *                 number of octets.  The domain name terminates with the
 *                 zero length octet for the null label of the root.  Note
 *                 that this field may be an odd number of octets; no
 *                 padding is used.
 *
 * QTYPE           a two octet code which specifies the type of the query.
 *                 The values for this field include all codes valid for a
 *                 TYPE field, together with some more general codes which
 *                 can match more than one type of RR.
 *
 * QCLASS          a two octet code that specifies the class of the query.
 *                 For example, the QCLASS field is IN for the Internet.
 */

public class DNSQuestion {

    private String[] QNAME_; // Store as String[] for ease of access and rewriting.
    private int QTYPE_;
    private int QCLASS_;

    /**
     * Read data passed by DNSMessage class and construct DNSQuestion
     * @param inputStream contains message received from Datagram Socket and Packet. What will be decoded
     * @param message used for methods within class, specifically for reading the domain name withing the question
     *                due to the need to handle compressed domain names.
     * @return a new DNSQuestion containing the values parsed from the inputStream.
     */
    static DNSQuestion decodeQuestion(InputStream inputStream, DNSMessage message) {
        DNSQuestion question = new DNSQuestion();
        byte[] data;
        question.QNAME_ = message.readDomainName(inputStream);
        try {
            data = inputStream.readNBytes(4);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        question.QTYPE_ = (((data[0] & 0xFF) << 8) | (data[1] & 0xFF));
        question.QCLASS_ = (((data[2] & 0xFF) << 8) | (data[3] & 0xFF));
        return question;
    }

    /**
     * Convert data stored in a DNSQuestion to bytes before sending back to client. Will be called in DNSMessage
     * @param outputStream where bytes will be written, allowing for passage between classes
     * @param domainNameLocations needed to determine where complete domain names will be used or compressed versions.
     *                            Will be passed to DNSMessage's method writeDomainName and not used here.
     */
    void writeBytes(ByteArrayOutputStream outputStream, HashMap<String,Integer> domainNameLocations) throws IOException {
        DNSMessage.writeDomainName(outputStream, domainNameLocations, this.QNAME_);
        byte[] data = new byte[4];
        data[0] = (byte)((this.QTYPE_ >> 8) & 0xFF);
        data[1] = (byte)(this.QTYPE_ & 0xFF);
        data[2] = (byte)((this.QCLASS_ >> 8) & 0xFF);
        data[3] = (byte)(this.QCLASS_ & 0xFF);
        outputStream.write(data);
    }

    @Override
    public String toString() {
        return "DNSQuestion{" +
                "QNAME_=" + Arrays.toString(QNAME_) +
                ", QTYPE_=" + QTYPE_ +
                ", QCLASS_=" + QCLASS_ +
                '}';
    }

    /**
     * Used in hashing for DNSCache
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof DNSQuestion that)) return false;
        return QTYPE_ == that.QTYPE_ && QCLASS_ == that.QCLASS_ && Arrays.equals(QNAME_, that.QNAME_);
    }

    /**
     * Used in hashing for DNSCache
     */
    @Override
    public int hashCode() {
        int result = Objects.hash(QTYPE_, QCLASS_);
        result = 31 * result + Arrays.hashCode(QNAME_);
        return result;
    }
}
