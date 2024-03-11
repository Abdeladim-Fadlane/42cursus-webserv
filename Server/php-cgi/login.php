<?php

// Define the correct usernames and passwords
$userCredentials = array(
    "afadlane" => "afadlane",
    "akatfi" => "akatfi"
);

// Check if form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Get the username and password from the form
    $username = $_POST["username"];
    $password = $_POST["password"];
    
    // Check if the username exists in the user credentials array
    if (array_key_exists($username, $userCredentials)) {
        // Check if the password matches the correct password for the given username
        if ($password === $userCredentials[$username]) {
            // Redirect to the respective page based on the username
            if ($username === "afadlane") {
                header("Location: afadlane.php");
                exit();
            } elseif ($username === "akatfi") {
                header("Location: akatfi.php");
                exit();
            }
        } else {
            // If password is incorrect, display an error message
            echo "Invalid password. Please try again.";
        }
    } else {
        // If username is not found, display an error message
        echo "Invalid username. Please try again.";
    }
}
?>
