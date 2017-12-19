#!/usr/bin/python3

# requirements: lvm vg test with lvm lv normal1


from sys import exit
from storage import *
from storageitu import *


set_logger(get_logfile_logger())

environment = Environment(False)

storage = Storage(environment)
storage.probe()

probed = storage.get_probed()

print(probed)

lvm_lv = to_lvm_lv(BlkDevice.find_by_name(probed, "/dev/test/normal1"))

resize_info = lvm_lv.detect_resize_info()

print(resize_info)

print(resize_info.resize_ok)
print(byte_to_humanstring(resize_info.min_size, False, 2, True))
print(byte_to_humanstring(resize_info.max_size, False, 2, True))
print(byte_to_humanstring(resize_info.block_size, False, 2, True))

