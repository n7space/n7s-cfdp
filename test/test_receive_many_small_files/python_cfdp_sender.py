import sys
import os.path
import filecmp

import cfdp
from time import sleep
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
udp_transport.bind("127.0.0.1", 5222)
source_filename1 = "/small1.txt"
destination_filename1 = "test/test_receive_many_small_files/target/received_small1.txt"
source_filename2 = "/small2.txt"
destination_filename2 = "test/test_receive_many_small_files/target/received_small2.txt"
source_filename3 = "/small3.txt"
destination_filename3 = "test/test_receive_many_small_files/target/received_small3.txt"

cfdp_entity = cfdp.CfdpEntity(
    entity_id=5, filestore=NativeFileStore("test/files"), transport=udp_transport
)

transaction_id = cfdp_entity.put(
    destination_id=6,
    source_filename=source_filename1,
    destination_filename=destination_filename1,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
)

while not cfdp_entity.is_complete(transaction_id):
    sleep(0.1)

transaction_id = cfdp_entity.put(
    destination_id=6,
    source_filename=source_filename2,
    destination_filename=destination_filename2,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
)

while not cfdp_entity.is_complete(transaction_id):
    sleep(0.1)

transaction_id = cfdp_entity.put(
    destination_id=6,
    source_filename=source_filename3,
    destination_filename=destination_filename3,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
)

while not cfdp_entity.is_complete(transaction_id):
    sleep(0.1)

if not os.path.isfile(destination_filename1):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not filecmp.cmp("test/files/" + source_filename1, destination_filename1):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not os.path.isfile(destination_filename2):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not filecmp.cmp("test/files/" + source_filename2, destination_filename2):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not os.path.isfile(destination_filename3):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not filecmp.cmp("test/files/" + source_filename3, destination_filename3):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

cfdp_entity.shutdown()
udp_transport.unbind()
sys.exit()