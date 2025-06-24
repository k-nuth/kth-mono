// Copyright (c) 2016-2023 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_DATABASE_GENERIC_DB_HPP_
#define KTH_DATABASE_GENERIC_DB_HPP_

#if defined(KTH_USE_LIBMDBX)
#include <mdbx.h>
#define KTH_DB_txn MDBX_txn
#define KTH_DB_val MDBX_val
#define KTH_DB_env MDBX_env
#define KTH_DB_dbi MDBX_dbi
#define KTH_DB_cursor MDBX_cursor

#define KTH_DB_SUCCESS MDBX_SUCCESS
#define KTH_DB_KEYEXIST MDBX_KEYEXIST
#define KTH_DB_RDONLY MDBX_RDONLY
#define KTH_DB_NOTFOUND MDBX_NOTFOUND
#define KTH_DB_SET MDBX_SET
#define KTH_DB_SET_RANGE MDBX_SET_RANGE
#define KTH_DB_NEXT MDBX_NEXT
#define KTH_DB_NORDAHEAD MDBX_NORDAHEAD
#define KTH_DB_NOSYNC MDBX_UTTERLY_NOSYNC       //TODO(fernando): check libmdbx sync modes
#define KTH_DB_NOTLS MDBX_NOTLS
#define KTH_DB_WRITEMAP MDBX_WRITEMAP
#define KTH_DB_MAPASYNC MDBX_MAPASYNC
#define KTH_DB_CREATE MDBX_CREATE
#define KTH_DB_INTEGERKEY MDBX_INTEGERKEY
#define KTH_DB_DUPSORT MDBX_DUPSORT
#define KTH_DB_NOOVERWRITE MDBX_NOOVERWRITE
#define KTH_DB_APPEND MDBX_APPEND
#define KTH_DB_LAST MDBX_LAST
#define KTH_DB_FIRST MDBX_FIRST
#define KTH_DB_DUPFIXED MDBX_DUPFIXED

#define kth_db_txn_commit mdbx_txn_commit
#define kth_db_cursor_close mdbx_cursor_close
#define kth_db_cursor_get mdbx_cursor_get
#define kth_db_cursor_del mdbx_cursor_del
#define kth_db_txn_abort mdbx_txn_abort
#define kth_db_dbi_close mdbx_dbi_close
#define kth_db_env_sync mdbx_env_sync
#define kth_db_txn_begin mdbx_txn_begin
#define kth_db_env_set_mapsize mdbx_env_set_mapsize
#define kth_db_env_create mdbx_env_create
#define kth_db_env_set_maxdbs mdbx_env_set_maxdbs
#define kth_db_env_open mdbx_env_open
#define kth_db_dbi_open mdbx_dbi_open
#define kth_db_put mdbx_put
#define kth_db_get mdbx_get
#define kth_db_cursor_open mdbx_cursor_open
#define kth_db_env_close mdbx_env_close
#define kth_db_del mdbx_del










inline
auto const& kth_db_get_data(KTH_DB_val const& x) {
    return x.iov_base;
}

inline
auto const& kth_db_get_size(KTH_DB_val const& x) {
    return x.iov_len;
}

inline
KTH_DB_val kth_db_make_value(size_t size, void* data) {
    return KTH_DB_val{data, size};
}



#else
#include <lmdb.h>
#define KTH_DB_txn MDB_txn
#define KTH_DB_val MDB_val
#define KTH_DB_env MDB_env
#define KTH_DB_dbi MDB_dbi
#define KTH_DB_cursor MDB_cursor

#define KTH_DB_SUCCESS MDB_SUCCESS
#define KTH_DB_KEYEXIST MDB_KEYEXIST
#define KTH_DB_RDONLY MDB_RDONLY
#define KTH_DB_NOTFOUND MDB_NOTFOUND
#define KTH_DB_SET MDB_SET
#define KTH_DB_SET_RANGE MDB_SET_RANGE
#define KTH_DB_NEXT MDB_NEXT
#define KTH_DB_NORDAHEAD MDB_NORDAHEAD
#define KTH_DB_NOSYNC MDB_NOSYNC
#define KTH_DB_NOTLS MDB_NOTLS
#define KTH_DB_WRITEMAP MDB_WRITEMAP
#define KTH_DB_MAPASYNC MDB_MAPASYNC
#define KTH_DB_CREATE MDB_CREATE
#define KTH_DB_INTEGERKEY MDB_INTEGERKEY
#define KTH_DB_DUPSORT MDB_DUPSORT
#define KTH_DB_NOOVERWRITE MDB_NOOVERWRITE
#define KTH_DB_APPEND MDB_APPEND
#define KTH_DB_LAST MDB_LAST
#define KTH_DB_FIRST MDB_FIRST
#define KTH_DB_DUPFIXED MDB_DUPFIXED



#define kth_db_txn_commit mdb_txn_commit
#define kth_db_cursor_close mdb_cursor_close
#define kth_db_cursor_get mdb_cursor_get
#define kth_db_cursor_del mdb_cursor_del
#define kth_db_txn_abort mdb_txn_abort
#define kth_db_dbi_close mdb_dbi_close
#define kth_db_env_sync mdb_env_sync
#define kth_db_txn_begin mdb_txn_begin
#define kth_db_env_set_mapsize mdb_env_set_mapsize
#define kth_db_env_create mdb_env_create
#define kth_db_env_set_maxdbs mdb_env_set_maxdbs
#define kth_db_env_open mdb_env_open
#define kth_db_dbi_open mdb_dbi_open
#define kth_db_put mdb_put
#define kth_db_get mdb_get
#define kth_db_cursor_open mdb_cursor_open
#define kth_db_env_close mdb_env_close
#define kth_db_del mdb_del

inline
auto const& kth_db_get_data(KTH_DB_val const& x) {
    return x.mv_data;
}

inline
auto const& kth_db_get_size(KTH_DB_val const& x) {
    return x.mv_size;
}

inline
KTH_DB_val kth_db_make_value(size_t size, void* data) {
    return KTH_DB_val{size, data};
}

#endif
#endif // KTH_DATABASE_GENERIC_DB_HPP_
