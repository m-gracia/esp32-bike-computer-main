<?php
/* 
 * Shows a map with the information stored on the DB
 * https://docs.mapbox.com/api/maps/
 * 
 * Parameters:
 * /m.php?
 * u=username&
 * p=password&
 * start=from time& //if not present return the last day. ej: 2019-05-17
 * end=to time // if not present return to the last one. ej: 2019-05-17
*/

if (isset($_GET['u']) && isset($_GET['p'])){
	//SQL connection
	try {
		$conn = new PDO ("mysql:host=localhost; dbname=<DATABASE>", "<USERNAME>","<STRONG_PASSWORD>");
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );
		}
	catch (PDOException $e ) {
			print("Error connecting to MySQL Server.");
			//die(print_r($e));
	}
	
	//Test vars
	if (isset($_GET['start'])) $start_date = $_GET['start'];
	else $start_date = gmdate("Y-m-d");
	$start_date .= "T00:00:00";
	if (isset($_GET['end'])) $end_date = $_GET['end'];
	else $end_date = gmdate("Y-m-d");
	$end_date .= "T23:59:59";
	
	//Auth select
	$getUser = $conn->prepare('SELECT id_users, passwd FROM bike_users
						WHERE id_users = ? AND passwd = ? AND writeperm = 0');
	$getUser->execute(array($_GET['u'],$_GET['p']));
	$user = $getUser->fetchAll(PDO::FETCH_ASSOC);
	if (count($user) > 0){
		//Web
		
?>
<!DOCTYPE html>
<html>
<head>
	<title>Route</title>

	<meta charset="utf-8" />
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	
	<link rel="shortcut icon" type="image/x-icon" href="docs/images/favicon.ico" />

    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.5.1/dist/leaflet.css" integrity="sha512-xwE/Az9zrjBIphAcBb3F6JVqxf46+CDLwfLMHloNu6KEQCAWi6HcDUbeOfBIptF7tcCzusKFjFw2yuvEpDL9wQ==" crossorigin=""/>
    <script src="https://unpkg.com/leaflet@1.5.1/dist/leaflet.js" integrity="sha512-GffPMF3RvMeYyc1LWMHtK8EbPv0iNZ8/oTtHPx9/cc2ILxQ+u905qIwdpULaqDkyBKgOaB57QTMg7ztg8Jm2Og==" crossorigin=""></script>

</head>
<body>
<div id="mapid" style="width: 100%; height: 600px;"></div>
<script>
	var mymap = L.map('mapid').setView([41.64, -0.869], 12);

	L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token=<MYMAPBOXTOKEN>', {
		tileSize: 512,
		maxZoom: 28,
		zoomOffset: -1,
		id: 'mapbox/streets-v11'
	}).addTo(mymap);

		<?php			
			try{
				$tsql = $conn->prepare("SELECT id_data AS ID, status AS Status, time AS Time, longitude AS Longitude, latitude AS Latitude, altitude AS Altitude 
					FROM bike_data WHERE time >= ? AND time <= ? ORDER BY time ASC");
				$tsql->execute(array($start_date,$end_date));
				$data_list = $tsql->fetchAll(PDO::FETCH_ASSOC);
				
				foreach ($data_list as $punto){
					if ($punto['Latitude'] != '0' && $punto['Longitude'] != '0'){
						echo "L.marker([".$punto['Latitude'].", ".$punto['Longitude']."], {title: '".$punto['Time']."',riseOnHover:true})";
						echo ".bindPopup('Tiempo: ".$punto['Time']."<br>Altitud: ".$punto['Altitude']."')";
						echo ".addTo(mymap);\n";
					}
				}
				
				echo "var puntosRuta = [";
				foreach ($data_list as $punto){
					if ($punto['Latitude'] != '0' && $punto['Longitude'] != '0'){
						echo "[".$punto['Latitude'].", ".$punto['Longitude']."],";
					}
				}
				echo "];\n";
				echo "var polyline = L.polyline(puntosRuta, {color: 'red'}).addTo(mymap);\n";
				echo "mymap.fitBounds(polyline.getBounds());\n";
				
			} catch (PDOException $e ) {
				print("Error saving ");
				die(print_r($e)); 
			}
			
		?>

</script>
</body>
</html>
<?php

	} else {
		print("Auth fail");
	}
	
}else{
	print("Unauthorized");
}

?>
