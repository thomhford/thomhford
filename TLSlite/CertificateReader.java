import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.security.InvalidKeyException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;

public class CertificateReader {
    /**
     * Reads in and creates Certificate from disk
     */
    public static Certificate readInCertificate(String filePath) throws CertificateException, FileNotFoundException {
        CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");
        return certificateFactory.generateCertificate(new FileInputStream(filePath));
    }

    /**
     * Probably unnecessary. Validates signature of CA Certificate. Our CA Certificate signed itself so basically
     * just checking that the CA Certificate has the right format
     */
    public static boolean verifyCertificate(Certificate certificate) throws Exception {
        Certificate CAcertificate = readInCertificate("Certificates/CAcertificate.pem");
        try {
            certificate.verify(CAcertificate.getPublicKey());
            return true;

        } catch (InvalidKeyException e) {
            return false;
        }
    }
}