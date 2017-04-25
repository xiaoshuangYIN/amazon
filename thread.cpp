#include "thread.h"

template <class T>

void to_string(T a, std::string& res){
  std::stringstream ss;
  ss << a;
  res = ss.str();
  ss.str("");
}
/*
 wid  | hid | cid | status_detail | track_num |         tid_list          | sid 
 */
void* pack_thread_func(void* para){
  thread_pack_para* para_pack = (thread_pack_para*)para;
  printf("It's me, thread %s!\n", (para_pack->id).c_str());
  int sid = 1;
  std::string status = std::string("pack0");
  while(true){
    // load shipment where sid = sid, status = pack0
    std::string sid_str;
    to_string(sid, sid_str);
    std::vector<std::unordered_map<std::string, std::string> >prods;
    uint32_t whnum;
    uint64_t shipid;
    bool ready = db_get_ship_topack(para_pack->C, sid_str, status, whnum, prods, shipid);
    if(ready == false){
      // continue;
      break;
    }
    // write Acommand

    // send message
    
    sid++;
  }
  pthread_exit(NULL);
}

void* load_thread_func(void* para){
  thread_load_para* para_load = (thread_load_para*)para;
  printf("It's me, thread %s!\n", (para_load->id).c_str());
  int sid = 1;
  while(true){
    sid++;
  }
  pthread_exit(NULL);
}

void* send_thread_func(void* para){
  //global:
  int purchase_id = 1;
  int wh_count = 10;
  int worldid = 1049;
  std::string s_wid;
  to_string(worldid, s_wid);
  thread_send_para* para_send = (thread_send_para*)para;
  printf("It's me, thread %s!\n", (para_send->id).c_str());
  int sockfd = para_send->sockfd;
  
  /* purchase more */ // change
  /*
  if(!send_APurchaseMore(whNum,  products, sockfd)){
    printf("send APurchaseMore failed\n");
  }
  */
  /* pack */ // change
  /*
  uint64_t shipId = 0;
  for(shipId = 1; shipId < 6; shipId++){
    if(!send_APack(whNum, shipId, pur, sockfd)){
      printf("send APack failed\n");
    }
  }
  */
  /* load */
  /*
  if(!send_APutOnTruck(sockfd, loads)){
      printf("send APutOnTruck failed\n");
  }
  */


  // load purchase with status 0(order placed)
  while(true){
    std::vector<std::unordered_map<std::string, std::string> > prod_array;
    std::string purchase_id_str;
    to_string(purchase_id, purchase_id_str);
    //std::cout<< purchase_id_str << std::endl;
    bool ready = db_get_purch(para_send->C, std::string("0"), purchase_id_str , prod_array);
    if(ready == false){
      continue;
    }
    //  for all product in this purchase
    for(int i = 0; i < prod_array.size(); i++){
      std::cout << (prod_array[i])["descrp"]<<std::endl;
      pq_t pq(comp(false));
      int total_num = 0;
      int sum = 0;
      int buy_count;
      std::stringstream((prod_array[i])["count"]) >> buy_count;
      printf("buy: %d\n", buy_count);

      // find the wharehouse number
      std::vector<std::unordered_map<std::string, int> > wh_map_array;
      db_get_hid(para_send->C, (prod_array[i])["pid"], wh_map_array);
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

	    to_string(m["hid"], s_hid);
	    s_pid = (prod_array[i])["pid"];
	    to_string(m["num"], s_num);
	    s_cid = (prod_array[i])["purchase_summary_id"];
	    db_add_ship_temp(
			     para_send->C,
			     s_wid,
			     s_hid,
			     s_pid,
			     s_num,
			     s_cid);
	    sum += m["num"];
	    printf("%d \n", m["num"]);
	    pq.pop();
	  }
	}// else: enough warehouse
      }// for every warehouse
      else{
	std::cout<< (prod_array[i])["decrp"] << " does not exist in warehouse\n";
      }
    }// for(products in one shipment) end
    
    // comb ship_temp into shipment(same purchase_id, same hid)
    std::vector<std::unordered_map<std::string, int> > ship_maps;
    std::vector<std::vector<std::unordered_map<std::string, std::string> > >prod_maps;
    db_add_shipments(para_send->C, s_wid, purchase_id_str, wh_count, ship_maps, prod_maps);
    
    // send Apack to sim
    for(int i = 0; i < ship_maps.size(); i++){
      if(!send_APack((ship_maps[i])["hid"], (ship_maps[i])["sid"] , prod_maps[i], para_send->sockfd)){
	printf("send APack failed\n");
      }
    }
    
    // send Apick to UPS

    // increment purchase_id(cid)
    purchase_id++;
    sleep(5);
  }// end while(true)
  pthread_exit(NULL);
}

void* recv_thread_func(void* para)
{
  thread_recv_para* para_recv = (thread_recv_para*)para;
  printf("It's me, thread %s!\n", (para_recv->id).c_str());
  AResponses res;
  /*
  while(true){
    recv_AResponse(para_recv->sockfd, res);
  }
  */
  /*test: add stock */
  std::string whnum("2");
  std::string pid("1");
  std::string num("20");
  std::string wid("1049");
  std::string descr("book");
  sleep(2);
  db_add_stock(para_recv->C, wid, whnum, pid,  num, descr);
  pthread_exit(NULL);
}
