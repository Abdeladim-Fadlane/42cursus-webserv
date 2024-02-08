<?php
// Set the Content-Type header to text/plain
header('Content-Type: text/plain');

// Set the redirect location
$redirect_url = 'https://profile.intra.42.fr/';

// Set the redirect header
header("Location: $redirect_url");
exit;
?>