

#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    PartitionTable::PartitionTable(Impl* impl)
	: Device(impl)
    {
    }


    PartitionTable::Impl&
    PartitionTable::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const PartitionTable::Impl&
    PartitionTable::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    Partition*
    PartitionTable::create_partition(const string& name)
    {
	return get_impl().create_partition(name);
    }


    Partition*
    PartitionTable::create_partition(unsigned int number)
    {
	return get_impl().create_partition(number);
    }


    void
    PartitionTable::delete_partition(const string& name)
    {
	return get_impl().delete_partition(name);
    }


    vector<const Partition*>
    PartitionTable::get_partitions() const
    {
	return get_impl().get_partitions();
    }


    Partition*
    PartitionTable::get_partition(const string& name)
    {
	return get_impl().get_partition(name);
    }


    const Disk*
    PartitionTable::get_disk() const
    {
	return get_impl().get_disk();
    }

}
