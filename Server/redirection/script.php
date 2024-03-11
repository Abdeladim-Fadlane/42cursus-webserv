<?php

// Retrieve the form data
$username = $_POST['username'] ?? '';

// If username is provided, redirect to the home page
if (!empty($username)) {
    // Concatenate the username with the URL
    $profile_url = "https://profile.intra.42.fr/users/" . urlencode($username);
    // Redirect to the profile page
    header("Location: $profile_url");
    exit();
}

// If username is missing, redirect back to the simple form page
header("Location: https://profile.intra.42.fr/");
exit();

?>
