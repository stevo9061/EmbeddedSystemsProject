<?php

$servername = "localhost";
$dbname = "vital_project";
$username = "root";
$password = "balou1407%";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $temp = test_input($_POST["temp"]);
        $heart_rate = test_input($_POST["heart_rate"]);
        
        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        } 
        
        $sql = "INSERT INTO VitalData (temp, heart_rate)
        VALUES ('" . $temp . "', '" . $heart_rate . "')";
        
        if ($conn->query($sql) === TRUE) {
            echo "New record created successfully";
        } 
        else {
            echo "Error: " . $sql . "<br>" . $conn->error;
        }
    
        $conn->close();
}
else {
    echo "No data posted with HTTP POST.";
}

function test_input($data) {
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
