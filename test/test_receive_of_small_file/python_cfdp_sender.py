import sys
import os.path
import filecmp

import cfdp
from time import sleep
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
udp_transport.bind("127.0.0.1", 5222)
source_filename = "/small1.txt"
destination_filename = "test/test_receive_of_small_file/target/received_small1.txt"

cfdp_entity = cfdp.CfdpEntity(
    entity_id=5, filestore=NativeFileStore("test/files"), transport=udp_transport
)

transaction_id = cfdp_entity.put(
    destination_id=6,
    source_filename=source_filename,
    destination_filename=destination_filename,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
)

while not cfdp_entity.is_complete(transaction_id):
    sleep(0.1)

if not os.path.isfile(destination_filename):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

if not filecmp.cmp("test/files/" + source_filename, destination_filename):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

cfdp_entity.shutdown()
udp_transport.unbind()
sys.exit()