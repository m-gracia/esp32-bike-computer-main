<?php 
/* 
 * Receives data from bike and stores on DB
 *
 * Parameters:
 * /p.php?
 * u=username&
 * p=password&
 * s=status&
 * l=longitude&
 * la=latitude&
 * a=altitude
 */

if (isset($_GET['u']) && isset($_GET['p'])){
	//SQL connection
	try {
		$conn = new PDO ("mysql:host=localhost; dbname=<DATABASE>", "<USERNAME>","<STRONG_PASSWORD>");
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );
		}
	catch (PDOException $e ) {
			print("Error connecting to MySQL Server.");
	}

	//Test size of vars
	// TODO

	//Auth select
	$getUser = $conn->prepare('SELECT id_users, passwd FROM bike_users
						WHERE id_users = ? AND passwd = ? AND writeperm = 1');
	$getUser->execute(array($_GET['u'],$_GET['p']));
	$user = $getUser->fetchAll(PDO::FETCH_ASSOC);
	if (count($user) > 0){
		//Telegram
		// TODO - Send alert by Telegram
		
		//Insert
		try{
			$tsql = $conn->prepare('INSERT INTO bike_data (status, time, longitude, latitude, altitude)
				VALUES (?,UTC_TIMESTAMP(),?,?,?,?,?,?)');
			$tsql->execute(array($_GET['s'],$_GET['l'],$_GET['la'],$_GET['a']));
		} catch (PDOException $e ) {
			print("Error saving ");
			die(print_r($e)); 
		}
		print("Done");
	
	} else {
		print("Auth fail");
	}
	
}else{
	print("Unauthorized");
}

?>