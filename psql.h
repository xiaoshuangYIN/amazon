#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <pqxx/pqxx>
#include <cstring>
#include <iterator>
#include <cstdio>
#include <cerrno>
#include <cstdlib>

using namespace pqxx;

connection* create_connection();

bool db_get_purch(connection* C, std::string status, std::string pur_id, std::vector<std::unordered_map<std::string, std::string> >& map_array );

void db_get_hid(connection* C, std::string pid, std::vector<std::unordered_map<std::string, int> >& map_array);

bool db_add_ship_temp(connection* C, std::string wid, std::string hid, std::string pid, std::string num_buy, std::string cid);
std::string get_tids_by_same_hid_cid(connection* C, std::string hid, std::string cid);

bool db_add_ship(connection* C, std::string wid,std::string hid, std::string cid, std::string tid_list);

bool  db_get_ship_topack(connection* C, int sockfd, std::string sid, std::string status, uint32_t& whnum, std::vector<std::unordered_map<std::string, std::string> >&prods, uint64_t& shipid);

bool db_get_ship_topack(
connection* C,
  std::string sid,
  std::string status,
  uint32_t& whnum,
  std::vector<std::unordered_map<std::string, std::string> >&prods,
uint64_t& shipid);

std::string db_get_cid_by_sid(connection* C, std::string sid);

bool  db_get_prods_by_wid(connection* C, std::string wid, std::vector<std::unordered_map<std::string, std::string> >& prods);
void db_get_package_info(connection* C, int sid, std::unordered_map<std::string, int>& package);
void db_get_Aload_info(connection* C, int sid, std::unordered_map<std::string, int>&load);
void db_get_sids_by_truckid_hid_status(connection* C, std::unordered_map<std::string, int>& truck_arrived, std::string ready_status, std::string not_ready_status, std::vector<int>& ready_list, std::vector<int>& not_ready_list);
std::vector<int> db_get_sids_by_hids_status(connection* C, int whid, std::string status);
// add
bool db_add_stock(connection* C,std::string wid, std::string whnum, std::string pid, std::string  num, std::string descr);


void db_add_shipments(connection* C, std::string wid, std::string cid, int wh_count, std::vector<std::unordered_map<std::string, int> >& ship_maps, std::vector<std::vector<std::unordered_map<std::string, std::string> > >& prod_maps);

void db_update_status_act_pur(connection* C, int sid, std::string status);

void db_update_status_shipment(connection* C, int sid, std::string status);
void db_add_truckid_to_shipment(connection* C, int truckid, int sid);
void db_add_truckid_to_shipment_by_whid(connection* C, int truckid, int whid);
void db_update_status_by_hids_status(connection* C, int whid, std::string status);
void db_add_trucknum_sctpur(connection* C, std::string sid, std::string cid);
void db_add_tracking(connection* C, int tracking, int sid);
// check
bool db_check_shipment_status(connection* C, int sid, std::string status);
