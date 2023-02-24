BEGIN {
    $scanning = 0;
    $triggered = 1;
    $copying = 2 ;
    $skipping = 3;
    $state = $scanning;
}

while (<>) {
    if ( $state == $scanning ) {
        $state = $copying if /-----BEGIN PRIVATE KEY-----/ ;
    }
    if ( $state == $copying ) {
        print;
        $state = $skipping if /-----END PRIVATE KEY-----/ ;
    }
}
