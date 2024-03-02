<?php
session_start();

// Check if the user is already logged in, redirect to welcome page if logged in

session_destroy();

// Redirect to login.php
header("Location: login.php");
// Check if the form is submitted
?>