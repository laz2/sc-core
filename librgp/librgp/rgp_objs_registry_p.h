/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

/**
 * @addtogroup librgp
 * @{
 */

/**
 * @file rgp_objs_registry_p.h
 * @brief Registry for system object which uses as RGP-object, like sc_addr's, sc_segment's and etc
 * @author Dmitry Lazurkin
 */

#ifndef __LIBRGP_RGP_REGISTRY_H_INCLUDED__
#define __LIBRGP_RGP_REGISTRY_H_INCLUDED__

namespace RGP {

/// Integer UID's generator for RGP-objects.
class uid_generator {
public:
	uid_generator() : max_id(1) {}

	rgp_object_id get_id() {
		rgp_object_id id;
		if (!free_ids.empty()) {
			id = free_ids.front();
			free_ids.pop_front();
		} else {
			id = max_id++;
		}
		return id;
	}

	void free_id(rgp_object_id id) {
		if (max_id - id == 1) {
			--max_id;
		} else {
			free_ids.push_back(id);
		}
	}

private:
	rgp_object_id max_id;
	std::list< rgp_object_id > free_ids;
};

/// Register/unregister mapping from RGP-object to UID.
template< typename ObjType >
class rgp_obj_registry {
public:
	typedef std::map< ObjType, rgp_object_id > obj2uid_map;
	typedef typename obj2uid_map::const_iterator const_iterator;

	/// Return RGP-object by UID. 0 if UID is 0 or object isn't registered.
	ObjType get(rgp_object_id uid) const {
		if (uid == 0)
			return 0;

		typename uid2obj_map::const_iterator it = uid2obj.find(uid);
		if (it != uid2obj.end()) {
			return it->second;
		} else {
			return 0;
		}
	}

	/// Return UID by RGP-object. If object not registered yet then register it.
	rgp_object_id get(ObjType obj) {
		if (!obj)
			return 0;

		typename obj2uid_map::const_iterator it = obj2uid.find(obj);
		if (it != obj2uid.end()) {
			return it->second;
		} else {
			rgp_object_id uid = gen.get_id();
			obj2uid.insert(typename obj2uid_map::value_type(obj, uid));
			uid2obj.insert(typename uid2obj_map::value_type(uid, obj));
			return uid;
		}
	}

	/// Unregister UID of RGP-object.
	void unregister(ObjType obj) {
		if (obj) {
			typename obj2uid_map::iterator it = obj2uid.find(obj);
			if (it != obj2uid.end()) {
				rgp_object_id uid = it->second;
				obj2uid.erase(it);

				typename uid2obj_map::iterator it2 = uid2obj.find(uid);
				assert(it2 != uid2obj.end());
				uid2obj.erase(it2);

				gen.free_id(uid);
			}
		}
	}

	const_iterator begin() const { return obj2uid.begin(); }

	const_iterator end() const { return obj2uid.end(); }

private:
	typedef std::map< rgp_object_id, ObjType > uid2obj_map;

	uid_generator gen;

	obj2uid_map obj2uid;
	uid2obj_map uid2obj;
};

/**
 * @brief Maps objects to RGP ids and RGP ids to objects.
 * Registry of sc_addr's, sc_segment's and other objects which sends with command packet.
 */
class rgp_objs_registry {
public:
	typedef rgp_obj_registry< sc_addr       > sc_addrs_registry;
	typedef rgp_obj_registry< sc_segment *  > sc_segments_registry;
	typedef rgp_obj_registry< sc_iterator * > sc_iterators_registry;
	typedef rgp_obj_registry< sc_wait *     > sc_waits_registry;
	typedef rgp_obj_registry< sc_activity * > sc_activities_registry;

	//
	// sc_addr
	//

	rgp_object_id get_uid(sc_addr obj) { return addrs.get(obj); }

	sc_addr get_sc_addr(rgp_object_id uid) const { return addrs.get(uid); }

	void unregister(sc_addr obj) { addrs.unregister(obj); }

	sc_addrs_registry& get_sc_addrs_registry() { return addrs; }

	//
	// sc_segment
	//

	rgp_object_id get_uid(sc_segment *obj) { return segments.get(obj); }

	sc_segment* get_sc_segment(rgp_object_id uid) const { return segments.get(uid); }

	void unregister(sc_segment *obj) { segments.unregister(obj); }

	sc_segments_registry& get_sc_segments_registry() { return segments; }

	//
	// sc_iterator
	//

	rgp_object_id get_uid(sc_iterator *obj) { return iterators.get(obj); }

	sc_iterator* get_sc_iterator(rgp_object_id uid) const { return iterators.get(uid); }

	void unregister(sc_iterator *obj) { iterators.unregister(obj); }

	sc_iterators_registry& get_sc_iterators_registry() { return iterators; }

	//
	// sc_wait
	//

	rgp_object_id get_uid(sc_wait *obj) { return waits.get(obj); }

	sc_wait* get_sc_wait(rgp_object_id uid) const { return waits.get(uid); }

	void unregister(sc_wait *obj) { waits.unregister(obj); }

	sc_waits_registry& get_sc_waits_registry() { return waits; }

	//
	// sc_activity
	//

	rgp_object_id get_uid(sc_activity *obj) { return activities.get(obj); }

	sc_activity* get_sc_activity(rgp_object_id uid) const { return activities.get(uid); }

	void unregister(sc_activity *obj) { activities.unregister(obj); }

	sc_activities_registry& get_sc_activities_registry() { return activities; }

private:
	sc_addrs_registry addrs;
	sc_segments_registry segments;
	sc_iterators_registry iterators;
	sc_waits_registry waits;
	sc_activities_registry activities;
};

} // namespace RGP

#endif // __LIBRGP_RGP_REGISTRY_H_INCLUDED__

/**@}*/
