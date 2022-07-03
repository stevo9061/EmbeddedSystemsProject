<?php

$servername = "localhost";
$dbname = "vital_project";
$username = "admin";
$password = "tXqcg8BVqp9R";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
} 

$sql = "SELECT id, temp, heart_rate, reading_time FROM VitalData ORDER BY id DESC LIMIT 1";

if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_temp = $row["temp"];
        $row_heart_rate = $row["heart_rate"];
        $row_reading_time = $row["reading_time"];
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo ';' . $row_id . ';' . $row_temp . ';' . $row_heart_rate . ';' . $row_reading_time . ';';
    }
    $result->free();
}

$conn->close();
?> 
