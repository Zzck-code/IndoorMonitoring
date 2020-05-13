<?php 
	require 'db.php';
 		
 		$sqltran = mysqli_query($con, "SELECT * FROM logs ORDER BY id DESC  ")or die(mysqli_error($con));
		$arrVal = array();
 		
		$i=1;
 		while ($rowList = mysqli_fetch_array($sqltran)) {
 								 
						$name = array(
								'num' => $i,
 	 		 	 				'humidity'=> $rowList['humidity'],
	 		 	 				'waterPump'=> $rowList['waterPump'],
	 		 	 				'date'=> $rowList['date'],
	 		 	 				'time'=> $rowList['time']
 	 		 	 			);		


							array_push($arrVal, $name);	
			$i++;			
	 	}
	 		 echo  json_encode($arrVal);		
 

	 	mysqli_close($con);
?>   
 