import os
import datetime
import binascii
import json
import argparse
import cryptography
from cryptography import x509
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec


from aws_kit_common import *

def main():
    csr = '3081fb3081a2020100302f31143012060355040a0c0b4578616d706c6520496e633117301506035504030c0e4578616d706c65204465766963653059301306072a8648ce3d020106082a8648ce3d0301070342000451b325eabdcb038bacd21df57c4113682c7025deb77d972c49850c243cec5cdcdb85d4907535d183cb32d79ea27558de506a326a65880cfa04c3151a69e5386ba011300f06092a864886f70d01090e31023000300a06082a8648ce3d0403020348003045022100a80ef9a1861e0752ed2573e723d95ebd0d092c927115c150f604c1d8f278eba102207087f24c315b66fb3dceb48271504a466eea9ed792a8c7af7917ae4596ed6b6c'
	
    # Create argument parser to document script use
    print('Provisions the kit by taking a CSR and returning signed certificates.')

    kit_info = read_kit_info()
	
    print('\nLoading root CA certificate')
    if not os.path.isfile(ROOT_CA_CERT_FILENAME):
        raise AWSZTKitError('Failed to find root CA certificate file, ' + ROOT_CA_CERT_FILENAME + '. Have you run ca_create_root first?')
    with open(ROOT_CA_CERT_FILENAME, 'rb') as f:
        print('    Loading from ' + f.name)
        root_ca_cert = x509.load_pem_x509_certificate(f.read(), crypto_be)

    print('\nLoading signer CA key')
    if not os.path.isfile(SIGNER_CA_KEY_FILENAME):
        raise AWSZTKitError('Failed to find signer CA key file, ' + SIGNER_CA_KEY_FILENAME + '. Have you run ca_create_signer_csr first?')
    with open(SIGNER_CA_KEY_FILENAME, 'rb') as f:
        print('    Loading from ' + f.name)
        signer_ca_priv_key = serialization.load_pem_private_key(
            data=f.read(),
            password=None,
            backend=crypto_be)

    print('\nLoading signer CA certificate')
    if not os.path.isfile(SIGNER_CA_CERT_FILENAME):
        raise AWSZTKitError('Failed to find signer CA certificate file, ' + SIGNER_CA_CERT_FILENAME + '. Have you run ca_create_signer first?')
    with open(SIGNER_CA_CERT_FILENAME, 'rb') as f:
        print('    Loading from ' + f.name)
        signer_ca_cert = x509.load_pem_x509_certificate(f.read(), crypto_be)

    if 'endpointAddress' not in kit_info:
        raise AWSZTKitError('endpointAddress not found in %s. Have you run aws_register_signer yet?' % KIT_INFO_FILENAME)


    print('\nGenerate device CSR')

    device_csr = x509.load_der_x509_csr(binascii.a2b_hex(csr), crypto_be)
    if not device_csr.is_signature_valid:
        raise AWSZTKitError('Device CSR has invalid signature.')
    with open(DEVICE_CSR_FILENAME, 'wb') as f:
        print('    Saving to ' + f.name)
        f.write(device_csr.public_bytes(encoding=serialization.Encoding.PEM))

    print('\nGenerating device certificate from CSR')
    # Build certificate
    builder = x509.CertificateBuilder()
    builder = builder.issuer_name(signer_ca_cert.subject)
    builder = builder.not_valid_before(datetime.datetime.now(tz=pytz.utc).replace(minute=0,second=0)) # Device cert must have minutes and seconds set to 0
    builder = builder.not_valid_after(datetime.datetime(3000, 12, 31, 23, 59, 59)) # Should be year 9999, but this doesn't work on windows
    builder = builder.subject_name(device_csr.subject)
    builder = builder.public_key(device_csr.public_key())
    # Device certificate is generated from certificate dates and public key
    builder = builder.serial_number(device_cert_sn(16, builder))
    # Add in extensions specified by CSR
    for extension in device_csr.extensions:
        builder = builder.add_extension(extension.value, extension.critical)
    # Subject Key ID is used as the thing name and MQTT client ID and is required for this demo
    builder = builder.add_extension(
        x509.SubjectKeyIdentifier.from_public_key(builder._public_key),
        critical=False)
    issuer_ski = signer_ca_cert.extensions.get_extension_for_class(x509.SubjectKeyIdentifier)
    builder = builder.add_extension(
        x509.AuthorityKeyIdentifier.from_issuer_subject_key_identifier(issuer_ski),
        critical=False)

    # Sign certificate 
    device_cert = builder.sign(
        private_key=signer_ca_priv_key,
        algorithm=hashes.SHA256(),
        backend=crypto_be)

    # Find the subject key ID for use as the thing name
    is_subject_key_id_found = False
    for extension in device_cert.extensions:
        if extension.oid._name != 'subjectKeyIdentifier':
            continue # Not the extension we're looking for, skip
        kit_info['thing_name'] = binascii.b2a_hex(extension.value.digest).decode('ascii')
        save_kit_info(kit_info)
        is_subject_key_id_found = True
    if not is_subject_key_id_found:
        raise RuntimeError('Could not find the subjectKeyIdentifier extension in the device certificate.')

    # Save certificate for reference
    with open(DEVICE_CERT_FILENAME, 'wb') as f:
        print('    Saving to ' + f.name)
        f.write(device_cert.public_bytes(encoding=serialization.Encoding.PEM))


    print('\nDone')

try:
    main()
except AWSZTKitError as e:
    print(e)