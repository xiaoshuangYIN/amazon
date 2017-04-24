#include "thread.h"


void* send_thread_func(void* para)
{

  /******* fetch from db ******/
  /*
  // buy
  uint32_t whNum = 1;
  std::vector<std::unordered_map<std::string, std::string> > products;
  std::unordered_map<std::string, std::string> map1;
  map1["id"] = "10";
  map1["description"]="cake";
  map1["count"]="5";
  std::unordered_map<std::string, std::string> map2;
  map2["id"] = "13";
  map2["description"]="candy";
  map2["count"]="5";
  products.push_back(map1);
  products.push_back(map2);
  // pack
  std::vector<std::unordered_map<std::string, std::string> > pur;
  std::unordered_map<std::string, std::string> m1;
  m1["id"] = "10";
  m1["description"]="cake";
  m1["count"]="1";
  std::unordered_map<std::string, std::string> m2;
  m2["id"] = "13";
  m2["description"]="candy";
  m2["count"]="1";
  pur.push_back(m1);
  pur.push_back(m2);
  // load
  std::vector<std::unordered_map<std::string, std::string> > loads;
  std::unordered_map<std::string, std::string> mm1;
  std::unordered_map<std::string, std::string> mm2;
  std::unordered_map<std::string, std::string> mm3;
  std::unordered_map<std::string, std::string> mm4;
  std::unordered_map<std::string, std::string> mm5;
  mm1["whNum"] = "1";
  mm1["shipId"]= "1";
  mm1["truckId"] = "1";
  mm2["whNum"] = "1";
  mm2["shipId"]= "2";
  mm2["truckId"] = "1";
  mm3["whNum"] = "1";
  mm3["shipId"]= "3";
  mm3["truckId"] = "1";
  mm4["whNum"] = "1";
  mm4["shipId"]= "4";
  mm4["truckId"] = "1";
  mm5["whNum"] = "1";
  mm5["shipId"]= "5";
  mm5["truckId"] = "1";
  loads.push_back(mm1);
  loads.push_back(mm2);
  loads.push_back(mm3);
  loads.push_back(mm4);
  loads.push_back(mm5);
  /* *********************** */
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
  std::vector<std::unordered_map<std::string, std::string> > prod_array;
  db_get_purch(para_send->C, std::string("0"), prod_array);

  for(int i = 0; i < prod_array.size(); i++){
    std::cout << (prod_array[i])["descrp"]<<std::endl;
    pq_t pq(comp(false));
    int total_num = 0;
    int sum = 0;
    int buy_count;
    std::stringstream((prod_array[i])["count"]) >> buy_count;
    // find the wharehouse number
    std::vector<std::unordered_map<std::string, int> > wh_map_array;
    db_get_hid(para_send->C, (prod_array[i])["pid"], wh_map_array);
    
    if(wh_map_array.size() > 0){
      for(int j = 0; j < wh_map_array.size(); j++){
	printf("%d\n",(wh_map_array[j])["num"]);
	//put into priority queue
	pq.push(wh_map_array[j]);
	total_num += (wh_map_array[j])["num"];
      }
      // check if enough stock for the product
      if(total_num < buy_count){
	std::cout<< "purchase id = " << (prod_array[i])["purid"] << "does not have sefficient stock in warehouses, failed\n";
      }
      // pop out untill the sum = num_buy
      else{
	while (sum < buy_count){
	  std::unordered_map<std::string, int> m = pq.top();
	  sum += m["num"];
	  printf("%d \n", m["num"]);
	  pq.pop();
	}
      }
    }
    else{
      std::cout<< (prod_array[i])["decrp"] << " does not exist in warehouse\n";
    }
  }
  // check warehouse number and put into ship_temp

  // for shipment

  // create 3 thread to send APack/ ALoad/ APick for rows in shipment
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
