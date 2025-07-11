import sys
import os.path
import filecmp

import cfdp
from time import sleep
from cfdp.transport.udp import UdpTransport
from cfdp.filestore import NativeFileStore

udp_transport = UdpTransport(routing={"*": [("127.0.0.1", 5111)]})
udp_transport.bind("127.0.0.1", 5222)
source_filename = ""
destination_filename = ""

cfdp_entity = cfdp.CfdpEntity(
    entity_id=5, filestore=NativeFileStore("test/files"), transport=udp_transport
)

transaction_id = cfdp_entity.put(
    destination_id=6,
    source_filename=source_filename,
    destination_filename=destination_filename,
    transmission_mode=cfdp.TransmissionMode.UNACKNOWLEDGED,
    messages_to_user=[
        cfdp.DirectoryListingRequest(
            remote_directory="test/files", local_file="listing_result.txt"
        )
    ],
)

sleep(2)

if not os.path.isfile("test/files/listing_result.txt"):
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

listing_content = open("test/files/listing_result.txt").read()

if 'big.txt' not in listing_content or 'small1.txt' not in listing_content:
    cfdp_entity.shutdown()
    udp_transport.unbind()
    sys.exit(-1)

cfdp_entity.shutdown()
udp_transport.unbind()
sys.exit()