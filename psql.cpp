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

void db_get_purch(connection* C, std::string status, std::vector<std::unordered_map<std::string, std::string> >& map_array ){
  try{
    std::string init_retrv = "SELECT * FROM account_purchase WHERE status= ";
    std::string post(";");
    std::string retrv_q = init_retrv  + status + post;
    nontransaction N(*C);
    result R(N.exec(retrv_q));
    std::unordered_map<std::string, std::string> map;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      map["purid"] = c[0].as<std::string>();
      map["pid"] = c[1].as<std::string>();
      map["descrp"] = c[2].as<std::string>();
      map["count"] = c[3].as<std::string>();
      map["status"] = c[4].as<std::string>();
      map_array.push_back(map);
    }
    
  }catch(const std::exception & e){
    std::cerr << e.what() << std::endl;
  }
}


void db_get_hid(connection* C, std::string pid, std::vector<std::unordered_map<std::string, int> >& map_array){
  try{
    
    std::string init_retrv = "SELECT hid, num FROM whstock  WHERE pid= ";
    std::string post(";");
    std::string retrv_q = init_retrv  + pid + post;
    nontransaction N(*C);
    result R(N.exec(retrv_q));

    
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
