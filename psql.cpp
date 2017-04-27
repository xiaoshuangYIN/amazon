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

bool db_get_purch(connection* C, std::string status, std::string purchase_id, std::vector<std::unordered_map<std::string, std::string> >& map_array ){
  try{
    
    std::string init_retrv = "SELECT * FROM account_purchase WHERE status= ";
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
      map["purchase_summary_id"] = c[9].as<std::string>();
      map["pid"] = c[1].as<std::string>();
      map["descrp"] = c[2].as<std::string>();
      map["count"] = c[3].as<std::string>();
      map["status"] = c[4].as<std::string>();
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
    
    std::string init_retrv = "SELECT hid, num FROM whstock  WHERE pid= ";
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

  while(std::getline(ss, token, ',')) {
    tid_vector.push_back(token);
  }
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
void db_get_prod_by_pid(connection* C, std::string pid, std::unordered_map<std::string, std::string>& map){
  try{
    std::string init_retrv( "SELECT product_id, description, count FROM account_product WHERE product_id = ");

    std::string post(";");
    std::string retrv_q = init_retrv + pid + post;
    work W(*C);
    //nontransaction N(*C);
    result R(W.exec(retrv_q));
    W.commit();
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      map["pid"] = c[0].as<std::string>();
      map["desc"] = c[1].as<std::string>();
      map["count"] = c[2].as<std::string>();
    }
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}

void db_get_prod_by_tid(connection* C, std::string tid , std::unordered_map<std::string, std::string>& prod){
  std::string  pid = db_get_pid_by_tid(C, tid);
  db_get_prod_by_pid(C, pid, prod);
}

void db_get_prods_by_tids(connection* C, std::string tid_list, std::vector<std::unordered_map<std::string, std::string> >& prods){
  std::vector<std::string> tid_vector;
  parse_str(tid_list, tid_vector);
  
  for(int i = 0; i < tid_vector.size(); i++){
    std::unordered_map<std::string, std::string> prod;
    db_get_prod_by_tid(C, tid_vector[i], prod);
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
  for(int i = 1; i <= wh_count; i++){
      std::stringstream ss;
      ss << i;
      std::string hid = ss.str();
      ss.str("");
      std::string tid_list =
	get_tids_by_same_hid_cid(C, hid, cid);
      if(tid_list != std::string("")){
	db_add_ship(C, wid, hid, cid, tid_list);
	// get products info
	std::vector<std::unordered_map<std::string, std::string> > prods;
	db_get_prods_by_tids(C, tid_list, prods);
	prod_maps.push_back(prods);
	// get whnum,shipid
	std::unordered_map<std::string, int> ship;
	db_get_ships_by_hidcid(C, hid, cid, ship);
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
      /*
      int num_act = db_check_prod(C, pid);
      if(num_act == num_stock){
	db_update_actprod(C, pid, new_num);
	printf("num_act_prod = %d\n", num_act);
      }
      else{
	printf("num_act_prod = %d\n", num_act);
	std::cerr<<"whstock and account_product not match\n";
	db_update_actprod(C, pid, new_num)
      }
      */
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
