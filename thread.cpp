#include "thread.h"

template <class T>
void to_string(T a, std::string& res){
  std::stringstream ss;
  ss << a;
  res = ss.str();
  ss.str("");
}
void* pack_thread_func(void* para){
  thread_pack_para* para_pack = (thread_pack_para*)para;
  printf("It's me, thread %s!\n", (para_pack->id).c_str());
  pthread_exit(NULL);
}
void* load_thread_func(void* para){
  thread_load_para* para_load = (thread_load_para*)para;
  printf("It's me, thread %s!\n", (para_load->id).c_str());
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
    }// for end
    
    // comb ship_temp into shipment(same purchase_id, same hid)
    db_add_shipments(para_send->C, s_wid, purchase_id_str, wh_count);
    

    // create 3 thread to send APack/ ALoad/ APick for rows in shipment
      pthread_t thread_pack;
      pthread_t thread_load;
      pthread_t thread_pickup;
      int rc1, rc2, rc3;
      //TO DO: create tread pick
      thread_pack_para pack_para;
      pack_para.id = std::string("pack");
      pack_para.C = para_send->C;
      thread_load_para load_para;
      load_para.id = std::string("load");
      load_para.C = para_send->C;
      rc1 = pthread_create(&thread_pack, NULL, pack_thread_func, &pack_para);
      if (rc1){
	printf("ERROR; return code from pthread_create() is %d\n", rc1);
	exit(-1);
      }
      rc2 = pthread_create(&thread_load, NULL, load_thread_func, &load_para);
      if (rc2){
	printf("ERROR; return code from pthread_create() is %d\n", rc2);
	exit(-1);
      }


  // increment purchase_id(cid)
    purchase_id++;

  }// end while(true)
  pthread_exit(NULL);
}

void* recv_thread_func(void* para)
{
  thread_recv_para* para_recv = (thread_recv_para*)para;
  printf("It's me, thread %s!\n", (para_recv->id).c_str());
  AResponses res;
  while(true){
    recv_AResponse(para_recv->sockfd, res);
  }
  pthread_exit(NULL);
}
