import java.util.HashMap;

public class DNSCache {
    /**
     *  Where answers will be stored for future responses
     */
    public static HashMap<DNSQuestion, DNSRecord> dnsCache = new HashMap<>();

    /**
     * Basically a getter for answers from dnsCache. Will also check for expired records and remove them if expired.
     * @param question which answer to pull from the cache
     * @return the record requested if not expired, and it exists, or return null if the record is expired or
     * does not exist
     */
    public static DNSRecord checkHashMap(DNSQuestion question){
        DNSRecord record = dnsCache.get(question);
        if (record == null){
            return null;
        } else if (record.isExpired()) {
            dnsCache.remove(question);
            return null;
        }
        return record;
    }

    /**
     * Set values in dnsCache for storage to use in future responses
     * @param question key value for where to store answer. Question corresponding to answer
     * @param record answer to be stored with question. Answer corresponding to question
     */
    public static void storeHashMap(DNSQuestion question, DNSRecord record){
        System.out.println("Storing record...");
        dnsCache.put(question, record);
    }
}
