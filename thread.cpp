#include "thread.h"

pthread_mutex_t ma;
pthread_mutex_t mu;

template <class T>
void to_string(T a, std::string& res){
  std::stringstream ss;
  ss << a;
  res = ss.str();
  ss.str("");
}

void* ship_thread_func(void* para){
  //global:
  thread_ship_para* para_ship = (thread_ship_para*)para;
  printf("It's me, thread %s!\n", (para_ship->id).c_str());


  int purchase_id = para_ship->purchase_id;  
  int wh_count = para_ship->wh_count;
  int worldid = para_ship->wh_count;
  std::string s_wid;
  to_string(worldid, s_wid);
  
  // load purchase with status 0(order placed)
  while(true){
    std::vector<std::unordered_map<std::string, std::string> > prod_array;
    std::string purchase_id_str;
    to_string(purchase_id, purchase_id_str);

    bool ready = db_get_purch(para_ship->C, std::string("0"), purchase_id_str , prod_array);
    if(ready == false){
      sleep(5);
      continue;
    }
    //  for all products in this purchase
    for(int i = 0; i < prod_array.size(); i++){
      pq_t pq(comp(false));
      int total_num = 0;
      int sum = 0;
      int buy_count;
      std::stringstream((prod_array[i])["count"]) >> buy_count;
      
      // find the wharehouse number
      std::vector<std::unordered_map<std::string, int> > wh_map_array;
      db_get_hid(para_ship->C, (prod_array[i])["pid"], wh_map_array);
      if(wh_map_array.size() > 0){
	for(int j = 0; j < wh_map_array.size(); j++){
	  //put into priority queue
	  pq.push(wh_map_array[j]);
	  total_num += (wh_map_array[j])["num"];
	}

	// check if enough stock for the product
	if(total_num < buy_count){
	  std::cout<< "purchase id = " << (prod_array[i])["purid"] << "does not have sefficient stock in warehouses.\n";
	}
	
	// pop out untill the sum = num_buy
	else{
	  while (sum < buy_count){
	    std::unordered_map<std::string, int> m = pq.top();
	    // insert into ship_temp
	    std::stringstream ss;
	    std::string  s_hid, s_pid, s_num, s_cid;
	    // if the first warehouse already has enough stock:
	    if(m["num"] > (buy_count - sum)){
	      to_string(buy_count, s_num);
	      sum += buy_count;
	    }
	    else{
	      to_string(m["num"], s_num);
	      sum += m["num"];
	    }
	    
	    to_string(m["hid"], s_hid);
	    s_pid = (prod_array[i])["pid"];
	    s_cid = (prod_array[i])["purchase_summary_id"];
	    db_add_ship_temp(
			     para_ship->C,
			     s_wid,
			     s_hid,
			     s_pid,
			     s_num,
			     s_cid);
	    // To DO upate: stock

	    pq.pop();
	  }
	}// else: enough warehouse
      }// for every warehouse
      else{
	std::cout<< (prod_array[i])["decrp"] << " does not exist in warehouse\n";
      }
    }// for(products in one shipment) end
    

    // comb ship_temp into shipment(same purchase_id, same hid)
    std::vector<std::unordered_map<std::string, int> > ship_maps;// for this purchase
    std::vector<std::vector<std::unordered_map<std::string, std::string> > >prod_maps;
    db_add_shipments(para_ship->C, s_wid, purchase_id_str, wh_count, ship_maps, prod_maps);


    // TO DO: create UCommands
     for(int i = 0; i < ship_maps.size(); i++){
       ACommands Acomd;
      for(int j = 0; j < prod_maps[i].size(); j++){
	std::cout<< ((prod_maps[i])[j])["desc"] << std::endl;
      }
      create_ACom_APack((ship_maps[i])["hid"], (ship_maps[i])["sid"] , prod_maps[i], Acomd);

      //lock
      pthread_mutex_lock(&ma);
      para_ship->queue->push_back(Acomd);
      pthread_mutex_unlock(&ma); 
      // unlock

      printf("sent APack, sid = %d\n", (ship_maps[i])["sid"]);
    }
     

    

    // increment purchase_id(cid)
    purchase_id++;
    sleep(5);
  }// end while(true)
  pthread_exit(NULL);
}


void* send_thread_func(void* para){
  thread_send_para* para_send = (thread_send_para*)para;
  printf("It's me, thread %s!\n", (para_send->id).c_str());
 
  while(true){

    // lock
    pthread_mutex_lock(&ma);
    int size = (*(para_send->queue)).size();
    if( size > 0){
      //google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(para_send->sockfd);

      sendMesgTo((*(para_send->queue))[0], para_send->outfile);
      
      (para_send->queue)->erase(((para_send->queue)->begin())+0);
    }
    pthread_mutex_unlock(&ma);
    // unlock
    
    
  }
 
  pthread_exit(NULL);
}

void* recv_thread_func(void* para)
{
  thread_recv_para* para_recv = (thread_recv_para*)para;
  printf("It's me, thread %s!\n", (para_recv->id).c_str());
  //AResponses res;
  /*
    while(true){
    recv_AResponse(para_recv->sockfd, res);
  }
  */
  
  do {

    std::vector<int> readys;
    std::vector<int> loadeds;
    std::vector<std::pair<int, std::vector<std::unordered_map<std::string, std::string> > > >arriveds;

    //AResponses Ares;
    if(recv_parse_AResponse(para_recv->sockfd, readys, loadeds, arriveds)){
      // update db
      // arrived
      for(int i = 0; i < arriveds.size(); i++){
	sleep(2);
	std::string whnum;
	std::string wid;
	to_string( para_recv->wid , wid);
	to_string(arriveds[i].first, whnum);
	for(int j = 0; j < (arriveds[i].second).size(); j++){
	  db_add_stock(para_recv->C, wid, whnum, ((arriveds[i].second)[j])["id"],  ((arriveds[i].second)[j])["count"], ((arriveds[i].second)[j])["description"]);
	}
      }
      // ready
      /*
      for(int i = 0; i < readys.size(); i++){

      }
      for(int i = 0; i < loadeds.size(); i++){

      }
      */
    }
    else {
      break;
    }
  }while(1);

    
  /*test: add stock */
  /*
  std::string whnum("2");
  std::string pid("1");
  // std::string num("20");
  std::string wid("1049");
  std::string descr("book");
  sleep(2);
  db_add_stock(para_recv->C, wid, whnum, pid,  20, descr);
  */

  
  pthread_exit(NULL);

}

void* buy_thread_func(void* para){
  thread_buy_para* para_buy = (thread_buy_para*)para;
  for (int i = 0; i < para_buy->wh_count; i++){
    std::vector<std::unordered_map<std::string, std::string> > prods;
    std::string hid;
    to_string(i, hid);
    bool ready = db_get_prods_by_wid(para_buy->C, hid, prods);
    if(ready == true){
      send_APurchaseMore(i, prods, para_buy->sockfd);
    }
  }
  pthread_exit(NULL);
}
