<?php
/* 
 * Shows a map with the information stored on the DB
 * - https://learn.microsoft.com/en-us/azure/azure-maps/quick-demo-map-app#prerequisites
 * - https://samples.azuremaps.com/ 
 * 
 * Parameters:
 * /m.php?
 * u=username&
 * p=password&
 * start=from time& //if not present return the last day. ej: 2019-05-17
 * end=to time // if not present return to the last one. ej: 2019-05-17
*/

/* ********************* */
/* CHANGE TO YOUR VALUES */
/* ********************* */
$AZUREMAPSSASTOKEN = 'COPY_YOUR_AZURE_MAPS_SAS_TOKEN_HERE';
$DB_HOST = 'localhost';
$DB_DATABASE = 'your_database_name';
$DB_USERNAME = 'mariadb_username';
$DB_PASSWORD = 'strong_password';
/* ***************************** */
/* DO NOT CHANGE BELOW THIS LINE */
/* ***************************** */

if (isset($_GET['u']) && isset($_GET['p'])){
	//SQL connection
	try {
		$conn = new PDO ("mysql:host=$DB_HOST; dbname=$DB_DATABASE", "$DB_USERNAME","$DB_PASSWORD");
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

    <!-- Add references to the Azure Maps Map control JavaScript and CSS files. -->
    <link href="https://atlas.microsoft.com/sdk/javascript/mapcontrol/3/atlas.min.css" rel="stylesheet" />
    <script src="https://atlas.microsoft.com/sdk/javascript/mapcontrol/3/atlas.min.js"></script>

	<script>
		var map, datasource, popup, symbolLayer;

		function getMap() {
			//Initialize a map instance.
			map = new atlas.Map('myMap', {
				center: [-0.88, 41.64],
				zoom: 11,
				view: 'Auto',
				//Add authentication details for connecting to Azure Maps.
				authOptions: {
					authType: 'subscriptionKey',
					subscriptionKey: '<?php echo($AZUREMAPSSASTOKEN); ?>'
				}
			});

			//Wait until the map resources are ready.
            map.events.add('ready', function () {
				//Create a data source and add it to the map.
				datasource = new atlas.source.DataSource();
				map.sources.add(datasource);

				<?php			
					try{
						$tsql = $conn->prepare("SELECT id_data AS ID, status AS Status, time AS Time, longitude AS Longitude, latitude AS Latitude, altitude AS Altitude 
							FROM bike_data WHERE time >= ? AND time <= ? ORDER BY time ASC");
						$tsql->execute(array($start_date,$end_date));
						$data_list = $tsql->fetchAll(PDO::FETCH_ASSOC);
						
						if (sizeof($data_list) > 0){
							echo "datasource.add([";
							foreach ($data_list as $punto){
								if ($punto['Latitude'] != '0' && $punto['Longitude'] != '0'){
									echo "new atlas.data.Feature(new atlas.data.Point([".$punto['Longitude'].", ".$punto['Latitude']."]), {";
									echo "date: '".$punto['Time']."',";
									echo "altitude: '".$punto['Altitude']."'}),";
								}
							}
							echo "]);\n";

							echo "datasource.add(new atlas.data.Feature(new atlas.data.LineString([";
							foreach ($data_list as $punto){
								if ($punto['Latitude'] != '0' && $punto['Longitude'] != '0'){
									echo "[".$punto['Longitude'].", ".$punto['Latitude']."],";
								}
							}
							echo "])));\n";
						}

					} catch (PDOException $e ) {
						print("Error reading ");
						die(print_r($e)); 
					}					
				?>
				
				//Load the custom image icon into the map resources.
				map.imageSprite.add('arrow-icon', 'purpleArrowRight.png');
				
				//Add a line layer for displaying the line.
				map.layers.add([
					//Add a line layer for displaying the line.
					new atlas.layer.LineLayer(datasource, null, {
                    	strokeColor: 'red',
                        strokeWidth: 3
					}),

					//Add a symbol layer for rendering the arrow along the line.
					new atlas.layer.SymbolLayer(datasource, null, {
						lineSpacing: 100,
						placement: 'line',
						iconOptions: {
							image: 'arrow-icon',
							allowOverlap: true,
							anchor: 'center',
							size: 0.8
						}
                    })
				]);
				
				//Add a layer for rendering point data as symbols.
				symbolLayer = new atlas.layer.SymbolLayer(datasource, null, { iconOptions: {allowOverlap: true}});
				map.layers.add(symbolLayer);

				//Create a popup but leave it closed so we can update it and display it later.
				popup = new atlas.Popup({
					position: [0, 0],
					pixelOffset: [0, -18]
				});

				//Close the popup when the mouse moves on the map.
				map.events.add('mousemove', closePopup);
				/**
				 * Open the popup on mouse move or touchstart on the symbol layer.
				 * Mouse move is used as mouseover only fires when the mouse initially goes over a symbol. 
				 * If two symbols overlap, moving the mouse from one to the other won't trigger the event for the new shape as the mouse is still over the layer.
				 */
				map.events.add('mousemove', symbolLayer, symbolHovered);
				map.events.add('touchstart', symbolLayer, symbolHovered);
			});
		}

		function closePopup() {
			popup.close();
		}

		function symbolHovered(e) {
			//Make sure the event occurred on a shape feature.
			if (e.shapes && e.shapes.length > 0) {
				var properties = e.shapes[0].getProperties();

				//Update the content and position of the popup.
				popup.setOptions({
					//Create the content of the popup.
					content: `<div style="padding:10px;"><b>${properties.date}</b><br/>Altitude:${properties.altitude}</div>`,
					position: e.shapes[0].getCoordinates(),
					pixelOffset: [0, -18]
				});

				//Open the popup.
				popup.open(map);
			}
		}
	</script>	

</head>
<body onload="getMap()">
	<div id="myMap" style="position:relative;width:100%;min-width:290px;height:600px;"></div>
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
