import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Header section format
 *
 * The header contains the following fields:
 *
 *                                     1  1  1  1  1  1
 *       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                      ID                       |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                    QDCOUNT                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                    ANCOUNT                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                    NSCOUNT                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                    ARCOUNT                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * where:
 *
 * ID              A 16 bit identifier assigned by the program that
 *                 generates any kind of query.  This identifier is copied
 *                 the corresponding reply and can be used by the requester
 *                 to match up replies to outstanding queries.
 *
 * QR              A one bit field that specifies whether this message is a
 *                 query (0), or a response (1).
 *
 * OPCODE          A four bit field that specifies kind of query in this
 *                 message.  This value is set by the originator of a query
 *                 and copied into the response.  The values are:
 *
 *                 0               a standard query (QUERY)
 *
 *                 1               an inverse query (IQUERY)
 *
 *                 2               a server status request (STATUS)
 *
 *                 3-15            reserved for future use
 *
 * AA              Authoritative Answer - this bit is valid in responses,
 *                 and specifies that the responding name server is an
 *                 authority for the domain name in question section.
 *
 *                 Note that the contents of the answer section may have
 *                 multiple owner names because of aliases.  The AA bit
 *                 corresponds to the name which matches the query name, or
 *                 the first owner name in the answer section.
 *
 * TC              TrunCation - specifies that this message was truncated
 *                 due to length greater than that permitted on the
 *                 transmission channel.
 *
 * RD              Recursion Desired - this bit may be set in a query and
 *                 is copied into the response.  If RD is set, it directs
 *                 the name server to pursue the query recursively.
 *                 Recursive query support is optional.
 *
 * RA              Recursion Available - this be is set or cleared in a
 *                 response, and denotes whether recursive query support is
 *                 available in the name server.
 *
 * Z               Reserved for future use.  Must be zero in all queries
 *                 and responses.
 *
 * RCODE           Response code - this 4 bit field is set as part of
 *                 responses.  The values have the following
 *                 interpretation:
 *
 *                 0               No error condition
 *
 *                 1               Format error - The name server was
 *                                 unable to interpret the query.
 *
 *                 2               Server failure - The name server was
 *                                 unable to process this query due to a
 *                                 problem with the name server.
 *
 *                 3               Name Error - Meaningful only for
 *                                 responses from an authoritative name
 *                                 server, this code signifies that the
 *                                 domain name referenced in the query does
 *                                 not exist.
 *
 *                 4               Not Implemented - The name server does
 *                                 not support the requested kind of query.
 *
 *                 5               Refused - The name server refuses to
 *                                 perform the specified operation for
 *                                 policy reasons.  For example, a name
 *                                 server may not wish to provide the
 *                                 information to the particular requester,
 *                                 or a name server may not wish to perform
 *                                 a particular operation (e.g., zone
 *                                 transfer) for particular data.
 *
 *                 6-15            Reserved for future use.
 *
 * QDCOUNT         an unsigned 16 bit integer specifying the number of
 *                 entries in the question section.
 *
 * ANCOUNT         an unsigned 16 bit integer specifying the number of
 *                 resource records in the answer section.
 *
 * NSCOUNT         an unsigned 16 bit integer specifying the number of name
 *                 server resource records in the authority records
 *                 section.
 *
 * ARCOUNT         an unsigned 16 bit integer specifying the number of
 *                 resource records in the additional records section.
 */

public class DNSHeader {
    private int ID_; // identifier assigned by the program that generates any kind of query
    private int QR_; // specifies whether this message is a query (0), or a response (1).
    private int OPCODE_; // specifies kind of query in this message
    private int AA_; // Authoritative Answer
    private int TC_; // TrunCation
    private int RD_; // Recursion Desired
    private int RA_; // Recursion Available
    private int Z_; // Reserved for future use. Must be zero in all queries and responses.
    private int RCODE_; // Response code
    private int QDCOUNT_; // number of entries in the question section
    private int ANCOUNT_; // number of resource records in the answer section
    private int NSCOUNT_; // number of name server resource records in the authority records section
    private int ARCOUNT_; // number of resource records in the additional records section
    public int getQDCOUNT_() {
        return QDCOUNT_;
    }
    public int getANCOUNT_() {
        return ANCOUNT_;
    }
    public int getNSCOUNT_() {
        return NSCOUNT_;
    }
    public int getARCOUNT_() {
        return ARCOUNT_;
    }
    public int getQR_() {
        return QR_;
    }

    /**
     * Reads header from InputStream passed from DNSMessage. Acts as a constructor for the DNSHeader class
     * @param inputStream passed from DNSMessage.
     * @return a new DNSHeader constructed from values from the InputStream
     */
    static DNSHeader decodeHeader(InputStream inputStream){
        DNSHeader headerIN = new DNSHeader();
        byte[] ID; // need 2 bytes
        byte[] flags; // need 2 bytes
        byte[] data; // need 8 bytes for remainder of header. Total of 12 bytes for header
        try {
            ID = inputStream.readNBytes(2);
            flags = inputStream.readNBytes(2);
            data = inputStream.readNBytes(8);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        headerIN.ID_ = (((ID[0] & 0xFF) << 8) | (ID[1] & 0xFF)); // first two bytes to int
        headerIN.QR_ = (flags[0] & 0x80) >> 7;
        headerIN.OPCODE_ = (flags[0] & 0x78) >> 3;
        headerIN.AA_ = (flags[0] & 0x04) >> 2;
        headerIN.TC_ = (flags[0] & 0x02) >> 1;
        headerIN.RD_ = (flags[0] & 0x01);
        headerIN.RA_ = (flags[1] & 0x80) >> 7;
        headerIN.Z_ = (flags[1] & 0x70) >> 4;
        headerIN.RCODE_ = (flags[1] & 0x0F);
        headerIN.QDCOUNT_ = (((data[0] & 0xFF) << 8) | (data[1] & 0xFF));
        headerIN.ANCOUNT_ = (((data[2] & 0xFF) << 8) | (data[3] & 0xFF));
        headerIN.NSCOUNT_ = (((data[4] & 0xFF) << 8) | (data[5] & 0xFF));
        headerIN.ARCOUNT_ = (((data[6] & 0xFF) << 8) | (data[7] & 0xFF));
        return headerIN;
    }

    /**
     * Sets values of DNSHeader to send with response
     * @param request contains fields to return
     * @param response passed from DNSMessage to fill to reply to client
     * @return response that has been passed in
     */
    static DNSHeader buildHeaderForResponse(DNSMessage request, DNSMessage response){
        DNSHeader headerResponse = response.getHeader_();
        DNSHeader headerRequest = request.getHeader_();
        headerResponse.ID_ = headerRequest.ID_; // response ID needs to match request
        headerResponse.QR_ = 1; // Response code
        headerResponse.OPCODE_ = headerRequest.OPCODE_; // match request
        headerResponse.AA_ = 0; // ignoring authoritative answer
        headerResponse.TC_ = 0; // not truncating
        headerResponse.RD_ = headerRequest.RD_;
        headerResponse.RA_ = headerRequest.RA_;
        headerResponse.Z_ = headerRequest.Z_;
        headerResponse.RCODE_ = 0; // sending to error messages
        headerResponse.QDCOUNT_ = headerRequest.QDCOUNT_; // send same number of questions as were asked
        headerResponse.ANCOUNT_ = 1; // always sending one answer
        headerResponse.NSCOUNT_ = 0;
        headerResponse.ARCOUNT_ = 1; // always send to dig additional answer
        return headerResponse;
    }

    /**
     * Converts values stored in DNSHeader to bytes to send to client
     * @param outputStream where we will write bytes
     * @throws IOException throws because I don't want to handle
     */
    void writeBytes(OutputStream outputStream) throws IOException {
        byte [] data = new byte [12];
        data[0] = (byte)((this.ID_ >> 8) & 0xFF);
        data[1] = (byte)(this.ID_ & 0xFF);
        data[2] = (byte)((this.QR_ << 7) | (byte)(this.OPCODE_ << 3) | (byte)(this.AA_ << 2)
                | (byte)(this.TC_ << 1) | (byte)(this.RD_));
        data[3] = (byte)((this.RA_ << 7) | (this.Z_ << 4) | this.RCODE_);
        data[4] = (byte)((this.QDCOUNT_ >> 8) & 0xFF);
        data[5] = (byte)(this.QDCOUNT_ & 0xFF);
        data[6] = (byte)((this.ANCOUNT_ >> 8) & 0xFF);
        data[7] = (byte)(this.ANCOUNT_ & 0xFF);
        data[8] = (byte)((this.NSCOUNT_ >> 8) & 0xFF);
        data[9] = (byte)(this.NSCOUNT_ & 0xFF);
        data[10] = (byte)((this.ARCOUNT_ >> 8) & 0xFF);
        data[11] = (byte)(this.ARCOUNT_ & 0xFF);
        outputStream.write(data);
    }

    @Override
    public String toString() {
        return "DNSHeader{" +
                "ID_=" + ID_ +
                ", QR_=" + QR_ +
                ", OPCODE_=" + OPCODE_ +
                ", AA_=" + AA_ +
                ", TC_=" + TC_ +
                ", RD_=" + RD_ +
                ", RA_=" + RA_ +
                ", Z_=" + Z_ +
                ", RCODE_=" + RCODE_ +
                ", ODCOUNT_=" + QDCOUNT_ +
                ", ANCOUNT_=" + ANCOUNT_ +
                ", NSCOUNT_=" + NSCOUNT_ +
                ", ARCOUNT_=" + ARCOUNT_ +
                '}';
    }
}
