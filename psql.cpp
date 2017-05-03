#include "psql.h"
template <class T>
void To_string(T a, std::string& res){
  std::stringstream ss;
  ss << a;
  res = ss.str();
  ss.str("");
}

connection* create_connection(){
  try{
    connection* C = new connection("dbname=amazon user=bitnami");
    if(C->is_open()){
      std::cout << "open db successfully: " << C->dbname() << std::endl;
    }
    else{
      std::cout << "cannot open db" << std::endl;
      exit(1);
    }
    return C;
  }
  catch(const std::exception &e){
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}
/*
product_id | description | count | price | x_coordinate | y_coordinate | status | tracking | purchase_summary_id
*/
bool db_get_purch(connection* C, std::string status, std::string purchase_id, std::vector<std::unordered_map<std::string, std::string> >& map_array ){
  try{
    
    std::string init_retrv = "SELECT product_id, description, count, x_coordinate,y_coordinate, status, purchase_summary_id  FROM account_purchase WHERE status= ";
    std::string post(";");
    std::string AND(" AND ");
    std::string mid(" purchase_summary_id  = ");
    std::string retrv_q = init_retrv  + status + AND + mid + purchase_id + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    std::unordered_map<std::string, std::string> map;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      map["pid"] = c[0].as<std::string>();
      map["descrp"] = c[1].as<std::string>();
      map["count"] = c[2].as<std::string>();
      map["delx"] = c[3].as<std::string>();
      map["dely"] = c[4].as<std::string>();
      map["status"] = c[5].as<std::string>();
      map["purchase_summary_id"] = c[6].as<std::string>();
      std::cout<<"test count " << map["descrp"] << " " << map["count"]<<std::endl;
      map_array.push_back(map);

    }
    if(map_array.size() == 0){
      return false;
    }
    return true;
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
}


void db_get_hid(connection* C, std::string pid, std::vector<std::unordered_map<std::string, int> >& map_array){
  try{
    
    std::string init_retrv = "SELECT hid, num FROM whstock WHERE pid= ";
    std::string post(";");
    std::string retrv_q = init_retrv  + pid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      std::unordered_map<std::string, int> map;
      map["hid"] = c[0].as<int>();
      map["num"] = c[1].as<int>();
      map_array.push_back(map);
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

bool db_add_ship_temp(connection* C, std::string wid, std::string hid, std::string pid, std::string num_buy, std::string cid){
  try{
    std::string table ("ship_temp ");
    std::string pre ("INSERT INTO ");
    std::string mid ("VALUES (" );
    std::string post(");");
    std::string comma (",");
    std::string insert_q("");
    std::cout<<"test " << pid << " " << num_buy << std::endl;
    insert_q = pre + table + mid
      + wid + comma
      + hid + comma
      + pid + comma
      + num_buy + comma
      + cid 
      + post;
    work W_create(*C);
    W_create.exec(insert_q);
    W_create.commit();
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
   
 }
/*
wid  | hid | pid | num_buy | cid | tid 
*/

std::string get_tids_by_same_hid_cid(connection* C, std::string hid, std::string cid){
  std::string empty("");
  try{
    std::string tid_list("");
    std::string init_retrv( "SELECT tid FROM ship_temp WHERE hid= ");
    std::string AND(" AND ");
    std::string mid(" cid = ");
    std::string post(";");
    std::string comma(",");
    std::string retrv_q = init_retrv + hid + AND + mid + cid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
   for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      tid_list += c[0].as<std::string>();
      tid_list += comma;
    }
    if(tid_list != empty){
      tid_list.pop_back();
    }
    return tid_list;
      
    
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;

    return empty;
  }
}
/*
 wid | hid | cid | status_detail | track_num | tid_list | sid 
-----+-----+-----+---------------+-----------+----------+----- 
*/
bool db_add_ship(connection* C, std::string wid, std::string hid, std::string cid, std::string tid_list){
  try{
    std::string pre ("INSERT INTO ");
    std::string table (" shipment ");
    std::string mid ("VALUES (" );
    std::string post(");");
    std::string comma (",");
    std::string insert_q("");
    std::string left("'{");
    std::string right("}'");
    std::string empty(" '' ");
    std::string status("order0");
    std::string qleft("'");
    std::string qright("'");
    insert_q = pre + table + mid
      + wid + comma
      + hid + comma
      + cid + comma 
      + qleft +status + qright + comma
      + empty + comma
      + left + tid_list + right 
      + post;
    // std::cout << "insert:" << insert_q << std::endl;
    work W_create(*C);
    W_create.exec(insert_q);
    W_create.commit();
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

void parse_str(std::string tid_list, std::vector<std::string>& tid_vector){
  std::istringstream ss(tid_list);
  std::string token;
  std::cout<<"*******\n";
  while(std::getline(ss, token, ',')) {
    tid_vector.push_back(token);
    std::cout <<"---------" <<std::endl;
    std::cout<<token<<std::endl;
  }
  std::cout<<"*******\n";
  
}

std::string db_get_pid_by_tid(connection* C, std::string tid){
  std::string pid("");
  try{
    std::string init_retrv( "SELECT pid FROM ship_temp WHERE tid= ");
    std::string post(";");
    std::string retrv_q = init_retrv + tid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      pid = c[0].as<std::string>();
    }
    return pid;
    
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return pid;
  }
}
// TO DO
/*
product_id | description | count 
*/
std::string db_get_buy_count_by_pid_cid(connection* C, std::string pid, std::string cid){
  std::string count = "";
  try{
  std::string init_retrv( "SELECT count from account_purchase WHERE product_id = ");
  
    std::string post(";");
    std::string AND(" AND ");
    std::string cids(" purchase_summary_id = ");
    std::string retrv_q = init_retrv + pid + AND + cids + cid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      count =  c[0].as<std::string>();
    }
    return count;
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_get_prod_by_pid(connection* C, std::string pid, std::unordered_map<std::string, std::string>& map, std::string cid){
  try{
    std::string init_retrv( "SELECT product_id, description  FROM account_product WHERE product_id = ");

    std::string post(";");
    std::string retrv_q = init_retrv + pid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      map["pid"] = c[0].as<std::string>();
      map["desc"] = c[1].as<std::string>();
      map["count"] = db_get_buy_count_by_pid_cid(C, map["pid"], cid);
      
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_get_prod_by_tid(connection* C, std::string tid , std::unordered_map<std::string, std::string>& prod, std::string cid){
  std::string  pid = db_get_pid_by_tid(C, tid);
  db_get_prod_by_pid(C, pid, prod, cid);
}

void db_get_prods_by_tids(connection* C, std::string tid_list, std::vector<std::unordered_map<std::string, std::string> >& prods, std::string cid){

  std::vector<std::string> tid_vector;
  //std::cout<<tid_list<<std::endl;
  parse_str(tid_list, tid_vector);
  //tid_vector.push_back(tid_list);
  
  for(int i = 0; i < tid_vector.size(); i++){
    std::unordered_map<std::string, std::string> prod;
    db_get_prod_by_tid(C, tid_vector[i], prod, cid);
    prods.push_back(prod);
  }
}

void db_get_ships_by_hidcid(connection* C, std::string hid, std::string cid, std::unordered_map<std::string, int>& ship){
  try{
    std::string init_retrv( "SELECT hid, sid FROM shipment WHERE ");
    std::string hids("hid = ");
    std::string cids("cid = ");
    std::string AND(" AND ");
    std::string post(";");
    std::string retrv_q = init_retrv + hids + hid + AND + cids + cid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      ship["hid"] = c[0].as<int>();
      ship["sid"] = c[1].as<int>();
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_add_shipments(connection* C, std::string wid, std::string cid, int wh_count, std::vector<std::unordered_map<std::string, int> >& ship_maps, std::vector<std::vector<std::unordered_map<std::string, std::string> > > & prod_maps){
  
  for(int i = 0; i < wh_count; i++){
      std::stringstream ss;
      ss << i;
      std::string hid = ss.str();
      ss.str("");

      // get tid_list, add to table shipment
      std::string tid_list =  // one shipment
	get_tids_by_same_hid_cid(C, hid, cid);
      if(tid_list != std::string("")){
	db_add_ship(C, wid, hid, cid, tid_list);

	// get products info 
	std::vector<std::unordered_map<std::string, std::string> > prods;
	db_get_prods_by_tids(C, tid_list, prods, cid);
	prod_maps.push_back(prods);//same cid, diff hid// all prods with same cid added

	// get whnum,shipid
	std::unordered_map<std::string, int> ship;//hid, sid
	db_get_ships_by_hidcid(C, hid, cid, ship);// after row added in shipment
	ship_maps.push_back(ship);
      }
  }
  return;
}
/*
 wid  | hid | cid | status_detail | track_num |         tid_list          | sid 
 */
bool db_get_ship_topack(
connection* C,
  std::string sid,
  std::string status,
  uint32_t& whnum,
  std::vector<std::unordered_map<std::string, std::string> >&prods,
  uint64_t& shipid){
  
  try{
    std::string init_retrv = "SELECT hid, tid_list, sid FROM shipment WHERE sid= ";
    std::string post(";");
    std::string AND(" AND ");
    std::string space(" ");
    std::string mid(" status_detail = ");
    std::string retrv_q = init_retrv + sid + AND + mid + status + post;
    work W(*C);
    //W_create.exec(retrv_q);
    

    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    std::string tid_list;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      whnum = c[0].as<int>();
      tid_list = c[1].as<std::string>();
      shipid = c[2].as<int>();
    }
    if(R.begin() != R.end()){
      return true;
    }
    else{
      return false;
    }
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
}

int db_check_stock_exist(connection* C, std::string wid, std::string hid, std::string pid){
  try{
    std::string init = "SELECT EXISTS (SELECT wid, hid, pid  FROM whstock WHERE ";
    std::string post = ");";
    std::string wids = "wid = ";
    std::string hids = "hid = ";
    std::string pids = "pid = ";
    std::string AND = " AND ";
    std::string retrieve_q = init + wids + wid + AND + hids + hid + AND + pids + pid + post;

    nontransaction N(*C);
    result R(N.exec(retrieve_q));
    bool res;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<bool>();
      if(res == true){
	return 1;
      }
      else if(res == false){
	return 0;
      }
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return 0;
  }
  return 1;
}
int db_check_stock(connection* C, std::string wid, std::string hid, std::string pid){
  try{
    std::string init = "SELECT num FROM whstock WHERE ";
    std::string post = ";";
    std::string wids = "wid = ";
    std::string hids = "hid = ";
    std::string pids = "pid = ";
    std::string AND = " AND ";
    std::string retrieve_q = init + wids + wid + AND + hids + hid + AND + pids + pid + post;

    nontransaction N(*C);
    result R(N.exec(retrieve_q));
    int res = -1;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<int>();
    }
    return res;
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

int db_check_prod(connection* C, std::string pid){
  try{
    std::string init = "SELECT count  FROM account_product WHERE ";
    std::string post = ";";
    std::string pids = "product_id = ";
    std::string retrieve_q = init + pids + pid + post;

    nontransaction N(*C);
    result R(N.exec(retrieve_q));
    int res = 0;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<int>();
    }
    return res;
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return -1;
  }
}

void db_update_actprod(connection* C, std::string pid, std::string num){
  try{
    std::string pre ("UPDATE ");
    std::string table (" account_product ");
    std::string set (" SET" );
    std::string counts(" count = ");
    std::string post(";");
    std::string comma (",");
    std::string empty(" '' ");
    std::string qleft("'");
    std::string qright("'");
    std::string where(" WHERE ");
    std::string AND(" AND ");
    std::string pids(" product_id = ");
    std::string  update_q = pre + table + set
      + counts + num
      + where
      + pids + pid
      + post;
    work W(*C);
    W.exec(update_q);
    W.commit();
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  } 
}

/*
id | product_id | description | count | price | num_of_ratings | stars 
*/
bool db_add_act_prod(connection* C, std::string num, std::string pid, std::string descr){
    try{

      
      std::string pre ("INSERT INTO ");
      std::string table (" account_product ");
      std::string mid ("VALUES (" );
      std::string post(");");
      std::string comma (",");
      std::string empty(" ");
      std::string empty_s(" '' ");
      std::string qleft("'");
      std::string qright("'");
      std::string price("5");
      std::string insert_q("");
      insert_q = pre + table + mid
	+ empty + comma
	+ pid + comma
	+ qleft + descr + qright + comma
	+ num + comma
	+ price + comma
	+ empty_s + comma
	+ empty_s 
	+ post;
      work W_create(*C);
      W_create.exec(insert_q);
      W_create.commit();
      return true;
    }
    catch(const std::exception & e){
      std::cerr << e.what() << std::endl;
      return false;
    }
}


/*
 wid  | hid | pid |   descr    | num 
*/
bool db_add_stock(connection* C,std::string wid, std::string whnum, std::string pid, std::string new_num, std::string descr){

  int num_stock = db_check_stock(C, wid, whnum, pid);
  printf("num_stock = %d\n", num_stock);

  if(num_stock >= 0){
    try{
      std::string num;
      int new_num_int;
      std::stringstream ss(new_num);
      ss >> new_num_int;
      To_string((new_num_int + num_stock), num);
      std::string pre ("UPDATE ");
      std::string table (" whstock ");
      std::string set (" SET" );
      std::string nums(" num = ");
      std::string post(";");
      std::string comma (",");
      std::string empty(" '' ");
      std::string qleft("'");
      std::string qright("'");
      std::string where(" WHERE ");
      std::string wids(" wid = ");
      std::string AND(" AND ");
      std::string hids(" hid = ");
      std::string pids(" pid = ");
      std::string  update_q = pre + table + set
	+ nums + num
	+ where
	+ wids + wid
	+ AND + hids + whnum
	+ AND + pids + pid
	+ post;
      work W(*C);
      W.exec(update_q);
      W.commit();

      // upadte account_product
  
      int num_act = db_check_prod(C, pid);
      if(num_act == num_stock){
	db_update_actprod(C, pid, new_num);
	printf("num_act_prod = %d\n", num_act);
      }
      else{
	printf("num_act_prod = %d\n", num_act);
	std::cerr<<"whstock and account_product not match\n";
	db_update_actprod(C, pid, new_num);
      }
  
      return true;
    }
    catch(const std::exception & e){
      std::cerr << e.what() << std::endl;
      return false;
    } 
  }
  else{
    try{
      
      std::string pre ("INSERT INTO ");
      std::string table (" whstock ");
      std::string mid ("VALUES (" );
      std::string post(");");
      std::string comma (",");
      std::string insert_q("");
      std::string empty(" '' ");
      std::string qleft("'");
      std::string qright("'");
      insert_q = pre + table + mid
	+ wid + comma
	+ whnum + comma
	+ pid + comma 
	+ qleft + descr + qright + comma
	+ new_num 
	+ post;
      work W_create(*C);
      W_create.exec(insert_q);
      W_create.commit();
      //db_add_act_prod(C, new_num, pid ,descr);
      return true;
    }
    catch(const std::exception & e){
      std::cerr << e.what() << std::endl;
      return false;
    }
  }
}
/*
wid  | hid | pid |   descr    | num
*/
bool  db_get_prods_by_wid(connection* C, std::string hid, std::vector<std::unordered_map<std::string, std::string> >& prods){
  try{
    
    std::string init_retrv = "SELECT pid, descr, num  FROM stock_to_add WHERE hid= ";
    std::string post(";");
    std::string retrv_q = init_retrv  + hid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    std::unordered_map<std::string, std::string> map;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      map["id"] = c[0].as<std::string>();
      map["description"] = c[1].as<std::string>();
      map["count"] = c[2].as<std::string>();
      prods.push_back(map);
    }
    if(prods.size() == 0){
      return false;
    }
    return true;
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
}
std::string db_get_cid_by_sid(connection* C, std::string sid){
  try{
    std::string res("");
    std::string init("SELECT cid FROM shipment WHERE sid = ");
    std::string post(";");
    std::string q = init + sid + post;
    work W(*C);
    result R(W.exec(q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<std::string>();
    }
    return res;
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}
void db_update_status_act_pur(connection* C, int sid, std::string status){
  try{
    std::string sid_str;
    To_string(sid, sid_str);
    std::string cid = db_get_cid_by_sid(C, sid_str);

    std::string init("UPDATE account_purchase SET ");
    std::string mid(" status =  ");
    std::string WHERE(" WHERE ");
    std::string cids(" purchase_summary_id = ");
    std::string post(";");
    std::string update_q("");

    update_q = init + mid + status + WHERE + cids +cid + post;
      
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
    
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}
bool db_check_shipment_status(connection* C, int sid, std::string status){
 try{
   std::string sid_str;
   To_string(sid, sid_str);
       
   std::string init = "SELECT EXISTS (SELECT sid, status_detail FROM shipment  WHERE ";
    std::string post = ");";
    std::string sids = "sid = ";
    std::string statuss = "status_detail = ";
    std::string lrq  = "'";
    std::string AND = " AND ";
    std::string retrieve_q = init + sids + sid_str + AND + statuss + lrq +
      status + lrq + post;

    nontransaction N(*C);
    result R(N.exec(retrieve_q));
    bool res;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<bool>();
      if(res == true){
	return true;
      }
      else if(res == false){
	return false;
      }
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
 }
}

void db_update_status_shipment(connection* C, int sid, std::string status){
try{
    std::string sid_str;
    To_string(sid, sid_str);

    std::string init("UPDATE shipment SET ");
    std::string mid(" status_detail =  ");
    std::string WHERE(" WHERE ");
    std::string sids(" sid = ");
    std::string lq("'");
    std::string rq("'");
    std::string post(";");
    std::string update_q("");

    update_q = init + mid  + lq + status + rq  + WHERE + sids +sid_str + post;
      
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
    
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
  
}

void db_get_xy_by_cid(connection* C, std::string cid, std::unordered_map<std::string, int>& package){

 try{
   std::string init("SELECT x_coordinate, y_coordinate FROM account_purchase WHERE purchase_summary_id = ");
    std::string post(";");
    std::string q = init + cid+ post;
    work W(*C);
    result R(W.exec(q));
    W.commit();
    result::const_iterator c = R.begin();
      package["delx"] = c[0].as<int>();
      package["dely"] = c[1].as<int>();
 }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

int  db_get_truckid_by_sid(connection* C, std::string sid_str){
  try{
    int res = 0;
    std::string init("SELECT truckid FROM shipment WHERE sid = ");
    std::string post(";");
    std::string q = init + sid_str + post;
    work W(*C);
    result R(W.exec(q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      res = c[0].as<int>();
    }
    return res;
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}
  
void db_get_package_info(connection* C, int sid, std::unordered_map<std::string, int>& package){

  std::string sid_str;
  To_string(sid, sid_str);
  std::string cid = db_get_cid_by_sid(C, sid_str);

  // truck id
  int truckid = db_get_truckid_by_sid(C, sid_str);
  package["truckid"] = truckid;
  printf("Udispatch truckid = %d\n", truckid);
  // x,y
  db_get_xy_by_cid(C, cid, package);
  printf("Udispatch x = %d\n", package["delx"]);
  printf("Udispatch y = %d\n", package["dely"]);
  

  // sid 
  package["sid"] = sid;
  printf("Udispatch sid = %d\n", sid);
}

void db_get_Aload_info(connection* C, int sid, std::unordered_map<std::string, int>&load){
  try{
    std::string sid_str;
    To_string(sid, sid_str);
    
    std::string init_retrv( "SELECT wid, truckid, sid FROM shipment WHERE ");
    std::string sids(" sid = ");
    std::string post(";");
    std::string retrv_q = init_retrv + sids + sid_str + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      load["wid"] = c[0].as<int>();
      load["truckid"] = c[1].as<int>();
      load["sid"] = c[2].as<int>();
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}


void db_get_sids_by_truckid_hid_status_not_ready(connection* C, std::unordered_map<std::string, int>& truck_arrived, std::string not_ready_status, std::vector<int>& not_ready_list){
  try{
    std::string truckid_str;
    To_string(truck_arrived["truckid"], truckid_str);
    std::string hid_str;
    To_string(truck_arrived["whid"], hid_str);
    
    std::string init_retrv( "SELECT sid FROM shipment WHERE ");
    std::string truckids(" truckid = ");
    std::string hids(" hid = ");
    std::string status(" status_detail = ");
    std::string AND(" AND ");
    std::string post(";");
    std::string lq("'");
    std::string rq("'");
    // ready
    std::string retrv_q = init_retrv + truckids + truckid_str + AND + hids + hid_str + AND + status +lq+ not_ready_status +rq + post;
    std::cout<< retrv_q << std::endl;
    work W(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      int sid = c[0].as<int>();
      not_ready_list.push_back(sid);
    }
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
  
}

void db_get_sids_by_truckid_hid_status(connection* C, std::unordered_map<std::string, int>& truck_arrived, std::string ready_status, std::string not_ready_status, std::vector<int>& ready_list, std::vector<int>& not_ready_list){
  try{
    std::string truckid_str;
    To_string(truck_arrived["truckid"], truckid_str);
    std::string hid_str;
    To_string(truck_arrived["whid"], hid_str);
    
    std::string init_retrv( "SELECT sid FROM shipment WHERE ");
    std::string truckids(" truckid = ");
    std::string hids(" hid = ");
    std::string status(" status_detail = ");
    std::string AND(" AND ");
    std::string post(";");
    std::string lq("'");
    std::string rq("'");
    // ready
    
    std::string retrv_q = init_retrv + truckids + truckid_str + AND + hids + hid_str + AND + status + lq +ready_status + rq + post;
    std::cout<< retrv_q << std::endl;
    work W(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      int sid = c[0].as<int>();
      ready_list.push_back(sid);
    }

    // not-ready
    db_get_sids_by_truckid_hid_status_not_ready( C, truck_arrived,  not_ready_status, not_ready_list);
      
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
  
}
void db_add_truckid_to_shipment(connection* C, int truckid, int sid){
  try{
    std::string sid_str;
    To_string(sid, sid_str);
    std::string truckid_str;
    To_string(truckid, truckid_str);
    
    std::string init("UPDATE shipment SET ");
    std::string mid(" truckid =  ");
    std::string lq("'");
    std::string rq("'");
    std::string WHERE(" WHERE ");
    std::string sids(" sid = ");
    std::string post(";");
    std::string update_q("");

    update_q = init + mid + truckid_str  + WHERE + sids +sid_str + post;
      
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
    
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_add_truckid_to_shipment_by_whid_not_ready(connection* C, int truckid, int whid){
  try{
    std::string hid_str;
    To_string(whid, hid_str);
    std::string truckid_str;
    To_string(truckid, truckid_str);

    
    std::string init("UPDATE shipment SET ");
    std::string mid(" truckid =  ");
    std::string WHERE(" WHERE ");
    std::string hids(" hid = ");
    std::string post(";");
    std::string update_q("");
    std::string later(" AND status_detail = 'order0' OR status_detail = 'ready'");
    update_q = init + mid + truckid_str + WHERE + hids +hid_str + later + post;
      
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
    
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_add_truckid_to_shipment_by_whid(connection* C, int truckid, int whid){
  try{
    std::string hid_str;
    To_string(whid, hid_str);
    std::string truckid_str;
    To_string(truckid, truckid_str);
    
    
    std::string init("UPDATE shipment SET ");
    std::string mid(" truckid =  ");
    std::string WHERE(" WHERE ");
    std::string hids(" hid = ");
    std::string post(";");
    std::string update_q("");
    std::string later(" AND status_detail = 'ready' OR status_detail = 'order0'");
    update_q = init + mid + truckid_str + WHERE + hids +hid_str  + post;
    std::cout<<update_q<<std::endl;
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
    //    db_add_truckid_to_shipment_by_whid_not_ready(C, truckid,  whid);
      
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

std::vector<int> db_get_sids_by_hids_status(connection* C, int whid, std::string status){
  try{
    std::vector<int> sids;
    std::string hid_str;
    To_string(whid, hid_str);
    
    std::string init_retrv( "SELECT sid FROM shipment WHERE ");
    std::string hids(" hid = ");
    std::string statuss(" status_detail = ");
    std::string AND(" AND ");
    std::string post(";");
    std::string lq("'");
    std::string rq("'");
    // ready
    
    std::string retrv_q = init_retrv + hids + hid_str + AND + statuss + lq + status + rq + post;
    std::cout<< retrv_q << std::endl;
    work W(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      
      int sid = c[0].as<int>();
      printf("ready ship to load sid=%d\n", sid);
      sids.push_back(sid);
    }
    return sids;
  }  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
  
  
}
void db_update_status_by_hids_status(connection* C, int whid, std::string status){
  try{
    std::string hid_str;
    To_string(whid, hid_str);

    std::string init("UPDATE shipment SET ");
    std::string mid(" status_detail =  ");
    std::string WHERE(" WHERE ");
    std::string hids(" hid = ");
    std::string post(";");
    std::string update_q("");
    std::string lq("'");
    std::string rq("'");
    std::string later(" AND status_detail = 'order0'");
    update_q = init + mid + lq+ status+rq + WHERE + hids +hid_str + later + post;
    std::cout<<update_q<<std::endl;
    work W(*C);
    result R(W.exec(update_q));
    W.commit();
      
  }
  catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}
     
