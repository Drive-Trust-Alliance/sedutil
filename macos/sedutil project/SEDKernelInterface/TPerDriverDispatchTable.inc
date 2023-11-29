
//userClient methods
__TPerDriverMethod__(kSedUserClientOpen,
                    
                        sOpenUserClient,	// Method pointer.
                        0,					// No scalar input values.
                        0,					// No struct input value.
                        0,					// No scalar output values.
                        0					// No struct output value.
                    )

__TPerDriverMethod__(kSedUserClientClose,

                        sCloseUserClient,	// Method pointer.
                        0,					//No scalar input values.
                        0,					//No struct input value.
                        0,					//No scalar output values.
                        0					//No struct output value.
                     )


// TPer Methods
__TPerDriverMethod__(kSedUserClientPerformSCSICommand,

                        sUserClientPerformSCSICommand,
                        6,					// 6 scalar input values, cdb[2], addresses/sizes of buffers
                        0,                  // No input struct.
                        1,   	    		// 1 scalar output value, address of actual size transferred
                        0                   // No output struct.
                     )


__TPerDriverMethod__(kSedUserClientUpdatePropertiesInIORegistry,

                        sUserClientUpdatePropertiesInIORegistry,
                        0,					// No scalar input values.
                        0,                  // No input struct.
                        0,   				// 0 scalar output values.
                        0                   // No output struct.
                     )


