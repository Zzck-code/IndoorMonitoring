<?php
    $server = "localhost";
    $username = "root";
    $password = "";
    $dbname = "irrigating";

    // Create connection
    $conn = new mysqli($server, $username, $password, $dbname);
    // Check connection
    if ($conn->connect_error) {
        die("Database Connection failed: " . $conn->connect_error);
    }

    date_default_timezone_set('Asia/Jakarta');
    $d = date("Y-m-d");
    $t = date("H:i:s");

    if(!empty($_POST['humidity']) && !empty($_POST['waterPump']))
    {
    	$humidity = $_POST['humidity'];
    	$waterPump = $_POST['waterPump'];

	    $sql = "INSERT INTO logs (humidity, waterPump, date, time)
		
		VALUES ('".$humidity."', '".$waterPump."', '".$d."', '".$t."')";

		if ($conn->query($sql) === TRUE) {
		    echo "OK";
		} else {
		    echo "Error: " . $sql . "<br>" . $conn->error;
		}
	}

	$conn->close();
?>