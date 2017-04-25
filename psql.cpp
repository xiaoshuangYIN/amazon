#include "psql.h"

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
      //std::cout<<"no rows found : db_get_purch\n";
      return false;
    }
    std::cout<<"rows found\n";
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
      + post;;
    std::cout << "insert:" << insert_q << std::endl;
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
    std::cout<<"test: tid = ";
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      tid_list += c[0].as<std::string>();
      tid_list += comma;
    }
    if(tid_list != empty){
      tid_list.pop_back();
    }
    std::cout<<tid_list<<std::endl;
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
    std::cout << "insert:" << insert_q << std::endl;
    work W_create(*C);
    W_create.exec(insert_q);
    W_create.commit();
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
    return false;
  }
  return true;
}

void db_add_shipments(connection* C, std::string wid, std::string cid, int wh_count){
  for(int i = 1; i <= wh_count; i++){
      std::stringstream ss;
      ss << i;
      std::string hid = ss.str();
      std::cout<<"test: hid = "<<hid<<std::endl;
      ss.str("");
      std::string tid_list =
	get_tids_by_same_hid_cid(C, hid, cid);
      if(tid_list != std::string("")){
	db_add_ship(C, wid, hid, cid, tid_list);
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
      std::cout<<"tid_list = " << tid_list << std::endl;
      shipid = c[2].as<int>();
      //db_get_prods_by_tid(C, prods, c[1].as<>);
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
/*
 wid  | hid | pid |   descr    | num 
*/
bool db_add_stock(connection* C,std::string wid, std::string whnum, std::string pid, std::string num, std::string descr){
  int exist = db_check_stock_exist(C, wid, whnum, pid);

  if(exist == 1){
    std::cout<< "exist!!!!!!!\n";
    try{
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
      std::cout << "update:" << update_q << std::endl;
      work W(*C);
      W.exec(update_q);
      W.commit();
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
	+ num 
	+ post;
      std::cout << "insert:" << insert_q << std::endl;
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
}
