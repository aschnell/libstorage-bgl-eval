/*
 * Copyright (c) 2017 SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include <iostream>

#include "storage/Devices/MultipathImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Exception.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageTypes.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/User.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Multipath>::classname = "Multipath";


    Multipath::Impl::Impl(const string& dm_name)
	: Partitionable::Impl(DEVMAPPERDIR "/" + dm_name), vendor(), model(), rotational(false)
    {
	set_dm_table_name(dm_name);
    }


    Multipath::Impl::Impl(const string& dm_name, const Region& region)
	: Partitionable::Impl(DEVMAPPERDIR "/" + dm_name, region, 256), vendor(), model(), rotational(false)
    {
	set_dm_table_name(dm_name);
    }


    Multipath::Impl::Impl(const xmlNode* node)
	: Partitionable::Impl(node), vendor(), model(), rotational(false)
    {
	getChildValue(node, "vendor", vendor);
	getChildValue(node, "model", model);

	getChildValue(node, "rotational", rotational);
    }


    bool
    Multipath::Impl::activate_multipaths(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_multipaths");

	if (!CmdMultipath(true).looks_like_real_multipath())
	{
	    y2mil("does not look like real multipath");
	    return false;
	}

	if (!activate_callbacks->multipath())
	{
	    y2mil("user canceled activation of multipath");
	    return false;
	}
	else
	{
	    y2mil("user allowed activation of multipath");
	}

	string cmd_line1 = MULTIPATHBIN;
	cout << cmd_line1 << endl;

	SystemCmd cmd1(cmd_line1);
	if (cmd1.retcode() != 0)
	    ST_THROW(Exception("activate multipath failed"));

	SystemCmd(UDEVADMBIN_SETTLE);

	string cmd_line2 = MULTIPATHDBIN;
	cout << cmd_line2 << endl;

	SystemCmd cmd2(cmd_line2);
	if (cmd2.retcode() != 0)
	    ST_THROW(Exception("activate multipath failed"));

	SystemCmd(UDEVADMBIN_SETTLE);

	return true;
    }


    void
    Multipath::Impl::probe_multipaths(Devicegraph* probed, SystemInfo& systeminfo)
    {
	const CmdMultipath& cmd_multipath = systeminfo.getCmdMultipath();

	for (const string& dm_name : cmd_multipath.get_entries())
	{
	    Multipath* multipath = Multipath::create(probed, dm_name);
	    multipath->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    Multipath::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Partitionable::Impl::probe_pass_1(probed, systeminfo);

	const File rotational_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/queue/rotational");
	rotational = rotational_file.get_int() != 0;

	const CmdMultipath& cmd_multipath = systeminfo.getCmdMultipath();

	const CmdMultipath::Entry& entry = cmd_multipath.get_entry(get_dm_table_name());

	vendor = entry.vendor;
	model = entry.model;

	for (const string& device : entry.devices)
	{
	    BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed, device, systeminfo);
	    User::create(probed, blk_device, get_device());
	}
    }


    uint64_t
    Multipath::Impl::used_features() const
    {
	return UF_MULTIPATH | Partitionable::Impl::used_features();
    }


    void
    Multipath::Impl::save(xmlNode* node) const
    {
	Partitionable::Impl::save(node);

	setChildValue(node, "vendor", vendor);
	setChildValue(node, "model", model);

	setChildValueIf(node, "rotational", rotational, rotational);
    }


    void
    Multipath::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot create multipath"));
    }


    void
    Multipath::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(Exception("cannot delete multipath"));
    }


    bool
    Multipath::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Partitionable::Impl::equal(rhs))
	    return false;

	return vendor == rhs.vendor && model == rhs.model && rotational == rhs.rotational;
    }


    void
    Multipath::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Partitionable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "vendor", vendor, rhs.vendor);
	storage::log_diff(log, "model", model, rhs.model);

	storage::log_diff(log, "rotational", rotational, rhs.rotational);
    }


    void
    Multipath::Impl::print(std::ostream& out) const
    {
	Partitionable::Impl::print(out);

	out << " vendor:" << vendor << " model:" << model;

	if (rotational)
	    out << " rotational";
    }


    void
    Multipath::Impl::process_udev_paths(vector<string>& udev_paths) const
    {
	// TODO ?
    }


    void
    Multipath::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	// TODO ?
    }


    bool
    compare_by_name(const Multipath* lhs, const Multipath* rhs)
    {
	return lhs->get_name() < rhs->get_name();
    }

}
