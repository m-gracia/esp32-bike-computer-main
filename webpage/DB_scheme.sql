CREATE TABLE IF NOT EXISTS bike_data (
	id_data INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
	status TINYINT,
	time DATETIME,
	longitude FLOAT,
	latitude FLOAT,
	altitude SMALLINT )
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS bike_users (
	id_users TINYINT PRIMARY KEY,
	writeperm BOOLEAN,
	passwd VARCHAR(50) )
ENGINE InnoDB;

CREATE TABLE IF NOT EXISTS bike_status (
	id_status TINYINT PRIMARY KEY,
	description VARCHAR(10) )
ENGINE InnoDB;

INSERT INTO bike_status (id_status,description) VALUES(0,"RUNNING");
INSERT INTO bike_status (id_status,description) VALUES(1,"POWERON");
INSERT INTO bike_status (id_status,description) VALUES(2,"NOTFOUND");
INSERT INTO bike_status (id_status,description) VALUES(3,"POWEROFF");

INSERT INTO bike_users (id_users, writeperm, passwd) VALUES (0,1,"<strong_password_for_rw_user>");
INSERT INTO bike_users (id_users, writeperm, passwd) VALUES (1,0,"<strong_password_for_ro_user>");
