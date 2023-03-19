import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.time.Instant;
import java.util.Arrays;
import java.util.HashMap;

/**
 * Resource record format
 *
 * The answer, authority, and additional sections all share the same
 * format: a variable number of resource records, where the number of
 * records is specified in the corresponding count field in the header.
 * Each resource record has the following format:
 *                                     1  1  1  1  1  1
 *       0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                                               |
 *     /                                               /
 *     /                      NAME                     /
 *     |                                               |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                      TYPE                     |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                     CLASS                     |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                      TTL                      |
 *     |                                               |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *     |                   RDLENGTH                    |
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
 *     /                     RDATA                     /
 *     /                                               /
 *     +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 *
 * where:
 *
 * NAME            a domain name to which this resource record pertains.
 *
 * TYPE            two octets containing one of the RR type codes.  This
 *                 field specifies the meaning of the data in the RDATA
 *                 field.
 *
 * CLASS           two octets which specify the class of the data in the
 *                 RDATA field.
 *
 * TTL             a 32 bit unsigned integer that specifies the time
 *                 interval (in seconds) that the resource record may be
 *                 cached before it should be discarded.  Zero values are
 *                 interpreted to mean that the RR can only be used for the
 *                 transaction in progress, and should not be cached.
 *
 * RDLENGTH        an unsigned 16 bit integer that specifies the length in
 *                 octets of the RDATA field.
 *
 * RDATA           a variable length string of octets that describes the
 *                 resource.  The format of this information varies
 *                 according to the TYPE and CLASS of the resource record.
 *                 For example, the if the TYPE is A and the CLASS is IN,
 *                 the RDATA field is a 4 octet ARPA Internet address.
 */

public class DNSRecord {
    private String[] NAME_;
    private int TYPE_;
    private int CLASS_;
    private long TTL_; // Stored in seconds
    private int RDLENGTH_;
    private byte[] RDATA_;
    private long timeStored_; // Stored in seconds


    /**
     * Read record and store values in DNSRecord from Google response
     * @param inputStream where we will be reading from
     * @param message used to read compressed domain names
     * @return a new DNSRecord now constructed with values from Google response
     */
    public static DNSRecord decodeRecord(InputStream inputStream, DNSMessage message) throws IOException {
        DNSRecord record = new DNSRecord();
        record.NAME_ = message.readDomainName(inputStream);
        byte[] data = inputStream.readNBytes(10);
        record.TYPE_ = (((data[0] & 0xFF) << 8) | (data[1] & 0xFF));
        record.CLASS_ = (((data[2] & 0xFF) << 8) | (data[3] & 0xFF));
        record.TTL_ = (((data[4] & 0xFF) << 24) | ((data[5] & 0xFF) << 16)
                | ((data[6] & 0xFF) << 8) | (data[7] & 0xFF));
        record.RDLENGTH_ = (((data[8] & 0xFF) << 8) | (data[9] & 0xFF));
        record.RDATA_ = inputStream.readNBytes(record.RDLENGTH_);
        record.timeStored_ = Instant.now().getEpochSecond();
        return record;
    }

    /**
     * Convert saved DNSRecord to bytes to be sent back to client
     * @param outputStream where bytes will be written
     * @param bytes HashMap containing location of complete, non-compressed, domain name to be used by
     *              DNSMessage.writeDomainName
     */
    public void writeBytes(ByteArrayOutputStream outputStream, HashMap<String, Integer> bytes) throws IOException {
        DNSMessage.writeDomainName(outputStream, bytes, this.NAME_);
        byte[] data = new byte[10];
        data[0] = (byte)((this.TYPE_ >> 8) & 0xFF);
        data[1] = (byte)(this.TYPE_ & 0xFF);
        data[2] = (byte)((this.CLASS_ >> 8) & 0xFF);
        data[3] = (byte)(this.CLASS_ & 0xFF);
        data[4] = (byte)(this.TTL_ >>> 24);
        data[5] = (byte)(this.TTL_ >>> 16);
        data[6] = (byte)(this.TTL_ >>> 8);
        data[7] = (byte)(this.TTL_);
        data[8] = (byte)((this.RDLENGTH_ >> 8) & 0xFF);
        data[9] = (byte)(this.RDLENGTH_ & 0xFF);
        outputStream.write(data);
        outputStream.write(this.RDATA_);
    }

    @Override
    public String toString() {
        return "DNSRecord{" +
                "NAME_=" + Arrays.toString(NAME_) +
                ", TYPE_=" + TYPE_ +
                ", CLASS_=" + CLASS_ +
                ", TTL_=" + TTL_ +
                ", RDLENGTH_=" + RDLENGTH_ +
                ", RDATA_=" + Arrays.toString(RDATA_) +
                '}';
    }

    /**
     * For use in DNSCache for checking for and removal of expired DNSRecords
     * @return true if TTL has passed, false if not yet passed
     */
    public boolean isExpired(){
        long currentTime = Instant.now().getEpochSecond();
        return currentTime > (TTL_ + timeStored_);
    }
}
