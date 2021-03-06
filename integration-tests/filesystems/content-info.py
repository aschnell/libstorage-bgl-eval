#!/usr/bin/python3

# requirements: partition /dev/sdc1 with a filesystem


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

staging = storage.get_staging()

print(staging)

partition = Partition.find_by_name(staging, "/dev/sdc1")
blk_filesystem = partition.get_blk_filesystem()

try:
    content_info = blk_filesystem.detect_content_info()
except Exception as exception:
    print(exception.what())
    exit(1)

print(content_info)

print(content_info.is_windows)
print(content_info.is_efi)
print(content_info.num_homes)

