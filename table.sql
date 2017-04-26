/* database name: amazon_db */

/* django */
/*DROP TABLE Product;
DROP TABLE CardPayment;
DROP TABLE Profile;
DROP TABLE GiftcardCode;
*/

/* amazon server */
DROP TABLE Warehouse;
DROP TABLE Whstock;
DROP TABLE Whready;
DROP TABLE Shipments;
DROP TABLE Purchased_Product;
DROP TABLE stock_to_add;
/* create tables */
CREATE TABLE stock_to_add (
       wid int8,
       hid int4,
       pid int8,
       descr text,
       num int8
);


CREATE TABLE Purchased_Product (
       cid int8,
       pid int8,
       des text,  
       num_buy int8,
       ship bool,
       status int2
);

CREATE TABLE Whstock (
        wid int8,
   	hid int4,
   	pid int8,
   	descr text,
   	num int8,
   	CONSTRAINT whpkey PRIMARY KEY (wid,hid,pid),
	UNIQUE (hid, pid)
);
 
CREATE TABLE Ship_temp (
       wid int8,
       hid int4,
       pid int8,
       num_buy int4,
       cid int8,
       tid SERIAL PRIMARY KEY
);

CREATE TABLE shipment (
       sid SERIAL PRIMARY KEY,	
       wid int8,
       hid int4,
       tid_list int8[],
       cid int8,
       status_detail text,
       track_num text
);


