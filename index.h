const char WEBpage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <body>
        <style>
        
        html {
            font-family: Arial;
            background-color: black;
            color: white;
        }
        h2, p {
            text-align: center;
        }
        p {
            color: red;
        }
        div {
            display: flex;
            justify-content: center;
            align-items: center;
        }
        button {
            user-select: none;
            -webkit-user-select: none;
            color: black;
            border-style: solid;
            border-width: 2px;
            border-color: white;
            margin-top: 2px;
            margin-left: 2px;
            margin-right: 2px;
            text-align: center;
            width: 120px;
            height: 35px;
            background-color: darkgrey;
        }
        button:hover {
            background-color: grey;
        }
        button:active {
            background-color: red;
        }
        input {
            border-style: solid;
            border-width: 2px;
            margin-top: 4px;
            margin-left: 4px;
            margin-right: 4px;
            text-align: center;
            width: 110px;
            height: 35px;
            background-color: white;
        }
        </style>
        
        <script>
            //manual buttons. This correlate to the buttons you see when in manual mode.
            var Lift1Up = false,Lift1Stop = false,Lift1Down = false,Lift2Up = false,Lift2Stop = false,Lift2Down = false,Lift3Up = false,
            Lift3Stop = false,Lift3Down = false,Motor4Forward = false,Motor4Stop = false,Motor4Back = false,Motor5Forward = false,
            Motor5Stop = false,Motor5Back = false,Motor6Forward = false,Motor6Stop = false,Motor6Back = false,Motor7Forward = false,
            Motor7Stop = false,Motor7Back = false,PickupOpen = false,PickupStop = false,PickupClose = false,TiltUp = false,TiltStop = false,
            TiltDown = false,BalanceForward = false,BalanceStop = false,BalanceBackward = false;
            //automatic buttons.
            var Forward = false, Backward = false, Right = false, Left = false, SRight = false, SLeft = false, FrontLiftUp = false, FrontLiftDown = false, MiddleLiftUp = false, MiddleLiftDown = false, BackLiftUp = false, BackLiftDown = false, Close = false, Open = false;

            //acceleration helpers
            var TargetPwmValue = 200;
            var AccelerationStep = 10;
            var ServoStep = 10;

            //for automatic only
            var AutoPwmValue = 80;
            var TurnPwmValue = 130;

            //Saved servo PWM values
            var PickupPwmValue = 90;

            //speed scalers for our wheels. These can be adjusted to ensure that the robot drives strait.
            var FrontRight = 0.9;
            var FrontLeft = 0.9;
            var BackRight = 0.9;
            var BackLeft = 0.9;

            // Mode of control. Only able to be in manual OR automatic. 
            // Automatic is the software-aided, less free form of control.
            // Manual is mostly for testing.
            var manual = false;

            //values we send to the robot
            //This are the values that the robot needs to function.
            var Motor1Direction = 1;//front lift
            var Motor1Speed = 0;
            var Motor2Direction = 1;//middle lift
            var Motor2Speed = 0;
            var Motor3Direction = 1;//back lift
            var Motor3Speed = 0;
            var Motor4Direction = 1;//back right motor
            var Motor4Speed = 0;
            var Motor5Direction = 1;//back left motor
            var Motor5Speed = 0;
            var Motor6Direction = 1;//front right motor
            var Motor6Speed = 0;
            var Motor7Direction = 1;//front left motor
            var Motor7Speed = 0;
            var PickupAngle = 90;
            var TiltAngle = 90;
            var BalanceAngle = 90;

            //for using acceleration with automatic control, we need some goal speed variables.
            var AimMotor1Speed = 0, AimMotor2Speed = 0, AimMotor3Speed = 0, AimMotor4Speed = 0, AimMotor5Speed = 0, AimMotor6Speed = 0, AimMotor7Speed = 0;

            //The following code runs 20 times a second. It process the controls, and sends that information through wifi to the robot.

            //All the values we send to the robot in a "compressed" form
            var dataToSend = "0000000000000000000000000000000000000";
            
            var gateway = `ws://${window.location.hostname}/ws`;
            var websocket;
            websocket = new WebSocket(gateway);
            setInterval(()=>{
                //using our keys and acceleration helpers, determine what data to send.
                //The next couple hundred lines are very boring, basically it just takes what keys you press and converts that into how we actually move the robot.

                if (manual == true) {
                    
                    if (Lift1Stop == true) {
                        Motor1Speed = 0;
                    } else if (Lift1Up == true) {
                        if (Motor1Speed < TargetPwmValue) {
                            Motor1Speed += AccelerationStep;
                            if (Motor1Speed > TargetPwmValue) {
                                Motor1Speed = TargetPwmValue;
                            }
                        }
                    } else if (Lift1Down == true) {
                        if (Motor1Speed > (TargetPwmValue*-1)) {
                            Motor1Speed -= AccelerationStep;
                            if (Motor1Speed < (TargetPwmValue*-1)) {
                                Motor1Speed = (TargetPwmValue*-1);
                            }
                        }
                    }
                    

                    if (Lift2Stop == true) {
                        Motor2Speed = 0;
                    } else if (Lift2Up == true) {
                        if (Motor2Speed < TargetPwmValue) {
                            Motor2Speed += AccelerationStep;
                            if (Motor2Speed > TargetPwmValue) {
                                Motor2Speed = TargetPwmValue;
                            }
                        }
                    } else if (Lift2Down == true) {
                        if (Motor2Speed > (TargetPwmValue*-1)) {
                            Motor2Speed -= AccelerationStep;
                            if (Motor2Speed < (TargetPwmValue*-1)) {
                                Motor2Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (Lift3Stop == true) {
                        Motor3Speed = 0;
                    } else if (Lift3Up == true) {
                        if (Motor3Speed < TargetPwmValue) {
                            Motor3Speed += AccelerationStep;
                            if (Motor3Speed > TargetPwmValue) {
                                Motor3Speed = TargetPwmValue;
                            }
                        }
                    } else if (Lift3Down == true) {
                        if (Motor3Speed > (TargetPwmValue*-1)) {
                            Motor3Speed -= AccelerationStep;
                            if (Motor3Speed < (TargetPwmValue*-1)) {
                                Motor3Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (Motor4Stop == true) {
                        Motor4Speed = 0;
                    } else if (Motor4Forward == true) {
                        if (Motor4Speed < TargetPwmValue) {
                            Motor4Speed += AccelerationStep;
                            if (Motor4Speed > TargetPwmValue) {
                                Motor4Speed = TargetPwmValue;
                            }
                        }
                    } else if (Motor4Back == true) {
                        if (Motor4Speed > (TargetPwmValue*-1)) {
                            Motor4Speed -= AccelerationStep;
                            if (Motor4Speed < (TargetPwmValue*-1)) {
                                Motor4Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (Motor5Stop == true) {
                        Motor5Speed = 0;
                    } else if (Motor5Forward == true) {
                        if (Motor5Speed < TargetPwmValue) {
                            Motor5Speed += AccelerationStep;
                            if (Motor5Speed > TargetPwmValue) {
                                Motor5Speed = TargetPwmValue;
                            }
                        }
                    } else if (Motor5Back == true) {
                        if (Motor5Speed > (TargetPwmValue*-1)) {
                            Motor5Speed -= AccelerationStep;
                            if (Motor5Speed < (TargetPwmValue*-1)) {
                                Motor5Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (Motor6Stop == true) {
                        Motor6Speed = 0;
                    } else if (Motor6Forward == true) {
                        if (Motor6Speed < TargetPwmValue) {
                            Motor6Speed += AccelerationStep;
                            if (Motor6Speed > TargetPwmValue) {
                                Motor6Speed = TargetPwmValue;
                            }
                        }
                    } else if (Motor6Back == true) {
                        if (Motor6Speed > (TargetPwmValue*-1)) {
                            Motor6Speed -= AccelerationStep;
                            if (Motor6Speed < (TargetPwmValue*-1)) {
                                Motor6Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (Motor7Stop == true) {
                        Motor7Speed = 0;
                    } else if (Motor7Forward == true) {
                        if (Motor7Speed < TargetPwmValue) {
                            Motor7Speed += AccelerationStep;
                            if (Motor7Speed > TargetPwmValue) {
                                Motor7Speed = TargetPwmValue;
                            }
                        }
                    } else if (Motor7Back == true) {
                        if (Motor7Speed > (TargetPwmValue*-1)) {
                            Motor7Speed -= AccelerationStep;
                            if (Motor7Speed < (TargetPwmValue*-1)) {
                                Motor7Speed = (TargetPwmValue*-1);
                            }
                        }
                    }

                    if (PickupStop == true) {
                        //servos only move when they aren't already in the required position.
                    } else if (PickupOpen == true) {
                        PickupAngle += ServoStep;
                    } else if (PickupClose == true) {
                        PickupAngle -= ServoStep;
                    }

                    if (TiltStop == true) {
                        //servos only move when they aren't already in the required position.
                    } else if (TiltUp == true) {
                        TiltAngle += ServoStep;
                    } else if (TiltDown == true) {
                        TiltAngle -= ServoStep;
                    }

                    if (BalanceStop == true) {
                        //servos only move when they aren't already in the required position.
                    } else if (BalanceForward == true) {
                        BalanceAngle += ServoStep;
                    } else if (BalanceBackward == true) {
                        BalanceAngle -= ServoStep;
                    }
                }
                if (manual == false) {
                    //automatic control
                    if (Forward == false && Backward == false && Right == false && Left == false && SLeft == false && SRight == false) {
                        //stop
                        AimMotor4Speed = 0;
                        AimMotor5Speed = 0;
                        AimMotor6Speed = 0;
                        AimMotor7Speed = 0;
                    }
                    if (Forward == true && Backward == false && Right == false && Left == false && SLeft == false && SRight == false) {
                        //move north
                        AimMotor4Speed = AutoPwmValue * BackRight;
                        AimMotor5Speed = AutoPwmValue * BackLeft;
                        AimMotor6Speed = AutoPwmValue * FrontRight;
                        AimMotor7Speed = AutoPwmValue * FrontLeft;
                    }
                    if (Forward == true && Backward == false && Right == false && Left == false && SLeft == false && SRight == true) {
                        //move north east
                        AimMotor7Speed = AutoPwmValue * FrontLeft;
                        AimMotor4Speed = AutoPwmValue * BackRight;

                        AimMotor5Speed = AutoPwmValue * 0;
                        AimMotor6Speed = AutoPwmValue * 0;
                    }
                    if (Forward == true && Backward == false && Right == false && Left == false && SLeft == true && SRight == false) {
                        //move north west
                        AimMotor5Speed = AutoPwmValue * BackLeft;
                        AimMotor6Speed = AutoPwmValue * FrontRight;

                        AimMotor7Speed = AutoPwmValue * 0;
                        AimMotor4Speed = AutoPwmValue * 0;
                    }
                    if (Forward == false && Backward == true && Right == false && Left == false && SLeft == false && SRight == false) {
                        //move south
                        AimMotor4Speed = AutoPwmValue * BackRight * -1;
                        AimMotor5Speed = AutoPwmValue * BackLeft * -1;
                        AimMotor6Speed = AutoPwmValue * FrontRight * -1;
                        AimMotor7Speed = AutoPwmValue * FrontLeft * -1;
                    }
                    if (Forward == false && Backward == true && Right == false && Left == false && SLeft == false && SRight == true) {
                        //move southeast
                        AimMotor5Speed = AutoPwmValue * BackLeft * -1;
                        AimMotor6Speed = AutoPwmValue * FrontRight * -1;

                        AimMotor7Speed = AutoPwmValue * 0;
                        AimMotor4Speed = AutoPwmValue * 0;
                    }
                    if (Forward == false && Backward == true && Right == false && Left == false && SLeft == true && SRight == false) {
                        //move southwest
                        AimMotor7Speed = AutoPwmValue * FrontLeft * -1;
                        AimMotor4Speed = AutoPwmValue * BackRight * -1;

                        AimMotor5Speed = AutoPwmValue * 0;
                        AimMotor6Speed = AutoPwmValue * 0;
                    }
                    if (Forward == false && Backward == false && Right == false && Left == false && SLeft == false && SRight == true) {
                        //move east
                        AimMotor7Speed = AutoPwmValue * FrontLeft;
                        AimMotor4Speed = AutoPwmValue * BackRight;

                        AimMotor5Speed = AutoPwmValue * BackLeft * -1;
                        AimMotor6Speed = AutoPwmValue * FrontRight * -1;
                    }
                    if (Forward == false && Backward == false && Right == false && Left == false && SLeft == true && SRight == false) {
                        //move west
                        AimMotor7Speed = AutoPwmValue * FrontLeft * -1;
                        AimMotor4Speed = AutoPwmValue * BackRight * -1;

                        AimMotor5Speed = AutoPwmValue * BackLeft;
                        AimMotor6Speed = AutoPwmValue * FrontRight;
                    }
                    if (Forward == true && Backward == false && Right == true && Left == false && SLeft == false && SRight == false) {
                        //move forward while turning right
                        AimMotor7Speed = TurnPwmValue * FrontLeft;
                        AimMotor5Speed = TurnPwmValue * BackLeft;

                        AimMotor4Speed = AutoPwmValue * 0;
                        AimMotor6Speed = AutoPwmValue * 0;
                    }
                    if (Forward == true && Backward == false && Right == false && Left == true && SLeft == false && SRight == false) {
                        //move forward while turning left
                        AimMotor7Speed = AutoPwmValue * 0;
                        AimMotor5Speed = AutoPwmValue * 0;

                        AimMotor4Speed = TurnPwmValue * BackRight;
                        AimMotor6Speed = TurnPwmValue * FrontRight;
                    }
                    if (Forward == false && Backward == true && Right == true && Left == false && SLeft == false && SRight == false) {
                        //move backward while turning right
                        AimMotor7Speed = TurnPwmValue * FrontLeft * -1;
                        AimMotor5Speed = TurnPwmValue * BackLeft * -1;

                        AimMotor4Speed = AutoPwmValue * 0;
                        AimMotor6Speed = AutoPwmValue * 0;
                    }
                    if (Forward == false && Backward == true && Right == false && Left == true && SLeft == false && SRight == false) {
                        //move backward while turning left
                        AimMotor7Speed = AutoPwmValue * 0;
                        AimMotor5Speed = AutoPwmValue * 0;

                        AimMotor4Speed = TurnPwmValue * BackRight * -1;
                        AimMotor6Speed = TurnPwmValue * FrontRight * -1;
                    }
                    if (Forward == false && Backward == false && Right == true && Left == false && SLeft == false && SRight == false) {
                        //turn right
                        AimMotor7Speed = TurnPwmValue * FrontLeft;
                        AimMotor5Speed = TurnPwmValue * BackLeft;

                        AimMotor4Speed = TurnPwmValue * BackRight * -1;
                        AimMotor6Speed = TurnPwmValue * FrontRight * -1;
                    }
                    if (Forward == false && Backward == false && Right == false && Left == true && SLeft == false && SRight == false) {
                        //turn left
                        AimMotor7Speed = TurnPwmValue * FrontLeft * -1;
                        AimMotor5Speed = TurnPwmValue * BackLeft * -1;

                        AimMotor4Speed = TurnPwmValue * BackRight;
                        AimMotor6Speed = TurnPwmValue * FrontRight;
                    }
                    

                    //lift controls
                    if (FrontLiftUp == true) {
                        AimMotor1Speed = AutoPwmValue;
                    } else if (FrontLiftDown == true) {
                        AimMotor1Speed = -1* AutoPwmValue;
                    } else {
                        AimMotor1Speed = 0;
                    }
                    if (MiddleLiftUp == true) {
                        AimMotor2Speed = AutoPwmValue;
                    } else if (MiddleLiftDown == true) {
                        AimMotor2Speed = -1* AutoPwmValue;
                    } else {
                        AimMotor2Speed = 0;
                    }
                    if (BackLiftUp == true) {
                        AimMotor3Speed = AutoPwmValue;
                    } else if (BackLiftDown == true) {
                        AimMotor3Speed = -1* AutoPwmValue;
                    } else {
                        AimMotor3Speed = 0;
                    }

                    //acceleration
                    if (Math.abs(AimMotor1Speed-Motor1Speed) >= 5) {
                        Motor1Speed += (AimMotor1Speed-Motor1Speed)/Math.abs(AimMotor1Speed-Motor1Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor2Speed-Motor2Speed) >= 5) {
                        Motor2Speed += (AimMotor2Speed-Motor2Speed)/Math.abs(AimMotor2Speed-Motor2Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor3Speed-Motor3Speed) >= 5) {
                        Motor3Speed += (AimMotor3Speed-Motor3Speed)/Math.abs(AimMotor3Speed-Motor3Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor4Speed-Motor4Speed) >= 5) {
                        Motor4Speed += (AimMotor4Speed-Motor4Speed)/Math.abs(AimMotor4Speed-Motor4Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor5Speed-Motor5Speed) >= 5) {
                        Motor5Speed += (AimMotor5Speed-Motor5Speed)/Math.abs(AimMotor5Speed-Motor5Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor6Speed-Motor6Speed) >= 5) {
                        Motor6Speed += (AimMotor6Speed-Motor6Speed)/Math.abs(AimMotor6Speed-Motor6Speed)*AccelerationStep;
                    }
                    if (Math.abs(AimMotor7Speed-Motor7Speed) >= 5) {
                        Motor7Speed += (AimMotor7Speed-Motor7Speed)/Math.abs(AimMotor7Speed-Motor7Speed)*AccelerationStep;
                    }

                    //pickup controls
                    if (Close == true) {
                        PickupPwmValue += 1;

                    } 
                    if (Open == true) {
                        PickupPwmValue -= 1;;
                    }
                    if (PickupPwmValue < 0) {
                        PickupPwmValue = 0;
                    }
                    if (PickupPwmValue > 180) {
                        PickupPwmValue = 180;
                    }
                    PickupAngle = PickupPwmValue;

                }
                //this code applies to both manual and automatic modes.
                if (Motor1Speed > 0) {
                    Motor1Direction = 2;
                } else {
                    Motor1Direction = 1;
                }
                if (Motor2Speed > 0) {
                    Motor2Direction = 2;
                } else {
                    Motor2Direction = 1;
                }
                if (Motor3Speed > 0) {
                    Motor3Direction = 2;
                } else {
                    Motor3Direction = 1;
                }
                if (Motor4Speed > 0) {
                    Motor4Direction = 2;
                } else {
                    Motor4Direction = 1;
                }
                if (Motor5Speed > 0) {
                    Motor5Direction = 2;
                } else {
                    Motor5Direction = 1;
                }
                if (Motor6Speed > 0) {
                    Motor6Direction = 2;
                } else {
                    Motor6Direction = 1;
                }
                if (Motor7Speed > 0) {
                    Motor7Direction = 2;
                } else {
                    Motor7Direction = 1;
                }
                

                //send finalized data to Robot
                dataToSend = String(Motor1Direction) + String(parseInt(Math.abs(Motor1Speed).toString())).padStart(3, '0') + 
                String(Motor2Direction) + String(parseInt(Math.abs(Motor2Speed).toString())).padStart(3, '0') + 
                String(Motor3Direction) + String(parseInt(Math.abs(Motor3Speed).toString())).padStart(3, '0') + 
                String(Motor4Direction) + String(parseInt(Math.abs(Motor4Speed).toString())).padStart(3, '0') + 
                String(Motor5Direction) + String(parseInt(Math.abs(Motor5Speed).toString())).padStart(3, '0') + 
                String(Motor6Direction) + String(parseInt(Math.abs(Motor6Speed).toString())).padStart(3, '0') + 
                String(Motor7Direction) + String(parseInt(Math.abs(Motor7Speed).toString())).padStart(3, '0') + 
                PickupAngle.toString().padStart(3, '0') + TiltAngle.toString().padStart(3, '0') + BalanceAngle.toString().padStart(3, '0');
                
                websocket.send(dataToSend);
                console.log(dataToSend);
            }, 50);
            //manual listeners
            function KeyDown(e) {
                if (e.code == "Digit1") {
                    Lift1Up = true;
                }
                if (e.code == "KeyQ") {
                    Lift1Stop = true;
                }
                if (e.code == "KeyA") {
                    Lift1Down = true;
                }
                if (e.code == "Digit2") {
                    Lift2Up = true;
                }
                if (e.code == "KeyW") {
                    Lift2Stop = true;
                }
                if (e.code == "KeyS") {
                    Lift2Down = true;
                }
                if (e.code == "Digit3") {
                    Lift3Up = true;
                }
                if (e.code == "KeyE") {
                    Lift3Stop = true;
                }
                if (e.code == "KeyD") {
                    Lift3Down = true;
                }
                if (e.code == "Digit4") {
                    Motor4Forward = true;
                }
                if (e.code == "KeyR") {
                    Motor4Stop = true;
                }
                if (e.code == "KeyF") {
                    Motor4Back = true;
                }
                if (e.code == "Digit5") {
                    Motor5Forward = true;
                }
                if (e.code == "KeyT") {
                    Motor5Stop = true;
                }
                if (e.code == "KeyG") {
                    Motor5Back = true;
                }
                if (e.code == "Digit6") {
                    Motor6Forward = true;
                }
                if (e.code == "KeyY") {
                    Motor6Stop = true;
                }
                if (e.code == "KeyH") {
                    Motor6Back = true;
                }
                if (e.code == "Digit7") {
                    Motor7Forward = true;
                }
                if (e.code == "KeyU") {
                    Motor7Stop = true;
                }
                if (e.code == "KeyJ") {
                    Motor7Back = true;
                }
                if (e.code == "Digit8") {
                    PickupOpen = true;
                }
                if (e.code == "KeyI") {
                    PickupStop = true;
                }
                if (e.code == "KeyK") {
                    PickupClose = true;
                }
                if (e.code == "Digit9") {
                    TiltUp = true;
                }
                if (e.code == "KeyO") {
                    TiltStop = true;
                }
                if (e.code == "KeyL") {
                    TiltDown = true;
                }
                if (e.code == "Digit0") {
                    BalanceForward = true;
                }
                if (e.code == "KeyP") {
                    BalanceStop = true;
                }
                if (e.code == "Semicolon") {
                    BalanceBackward = true;
                }
            }
            function KeyUp(e) {
                if (e.code == "Digit1") {
                    Lift1Up = false;
                }
                if (e.code == "KeyQ") {
                    Lift1Stop = false;
                }
                if (e.code == "KeyA") {
                    Lift1Down = false;
                }
                if (e.code == "Digit2") {
                    Lift2Up = false;
                }
                if (e.code == "KeyW") {
                    Lift2Stop = false;
                }
                if (e.code == "KeyS") {
                    Lift2Down = false;
                }
                if (e.code == "Digit3") {
                    Lift3Up = false;
                }
                if (e.code == "KeyE") {
                    Lift3Stop = false;
                }
                if (e.code == "KeyD") {
                    Lift3Down = false;
                }
                if (e.code == "Digit4") {
                    Motor4Forward = false;
                }
                if (e.code == "KeyR") {
                    Motor4Stop = false;
                }
                if (e.code == "KeyF") {
                    Motor4Back = false;
                }
                if (e.code == "Digit5") {
                    Motor5Forward = false;
                }
                if (e.code == "KeyT") {
                    Motor5Stop = false;
                }
                if (e.code == "KeyG") {
                    Motor5Back = false;
                }
                if (e.code == "Digit6") {
                    Motor6Forward = false;
                }
                if (e.code == "KeyY") {
                    Motor6Stop = false;
                }
                if (e.code == "KeyH") {
                    Motor6Back = false;
                }
                if (e.code == "Digit7") {
                    Motor7Forward = false;
                }
                if (e.code == "KeyU") {
                    Motor7Stop = false;
                }
                if (e.code == "KeyJ") {
                    Motor7Back = false;
                }
                if (e.code == "Digit8") {
                    PickupOpen = false;
                }
                if (e.code == "KeyI") {
                    PickupStop = false;
                }
                if (e.code == "KeyK") {
                    PickupClose = false;
                }
                if (e.code == "Digit9") {
                    TiltUp = false;
                }
                if (e.code == "KeyO") {
                    TiltStop = false;
                }
                if (e.code == "KeyL") {
                    TiltDown = false;
                }
                if (e.code == "Digit0") {
                    BalanceForward = false;
                }
                if (e.code == "KeyP") {
                    BalanceStop = false;
                }
                if (e.code == "Semicolon") {
                    BalanceBackward = false;
                }
            }
            document.addEventListener("keydown", KeyDown);
            document.addEventListener("keyup", KeyUp);
            //automatic listeners
            document.addEventListener("keydown", (e)=>{
                if (e.code == "KeyM") {
                    if (manual == true) {
                        manual = false;
                        alert("Automatic Mode");
                    } else {
                        manual = true;
                        alert("Manual Mode");
                    }
                }

                if (e.code == "ArrowUp") {
                    Forward = true;
                }
                if (e.code == "ArrowDown") {
                    Backward = true;
                }
                if (e.code == "ArrowRight") {
                    Right = true;
                }
                if (e.code == "ArrowLeft") {
                    Left = true;
                }
                if (e.code == "Comma") {
                    SLeft = true;
                }
                if (e.code == "Period") {
                    SRight = true;
                }
                if (e.code == "Digit1") {
                    FrontLiftUp = true;
                }
                if (e.code == "Digit2") {
                    MiddleLiftUp = true;
                }
                if (e.code == "Digit3") {
                    BackLiftUp = true;
                }
                if (e.code == "KeyQ") {
                    FrontLiftDown = true;
                }
                if (e.code == "KeyW") {
                    MiddleLiftDown = true;
                }
                if (e.code == "KeyE") {
                    BackLiftDown = true;
                }
                
                if (e.code == "Space") {
                    Close = true;
                }
                if (e.code == "KeyZ") {
                    Open = true;
                }
            });
            document.addEventListener("keyup", (e)=>{
                if (e.code == "ArrowUp") {
                    Forward = false;
                }
                if (e.code == "ArrowDown") {
                    Backward = false;
                }
                if (e.code == "ArrowRight") {
                    Right = false;
                }
                if (e.code == "ArrowLeft") {
                    Left = false;
                }
                if (e.code == "Comma") {
                    SLeft = false;
                }
                if (e.code == "Period") {
                    SRight = false;
                }

                if (e.code == "Digit1") {
                    FrontLiftUp = false;
                }
                if (e.code == "Digit2") {
                    MiddleLiftUp = false;
                }
                if (e.code == "Digit3") {
                    BackLiftUp = false;
                }
                if (e.code == "KeyQ") {
                    FrontLiftDown = false;
                }
                if (e.code == "KeyW") {
                    MiddleLiftDown = false;
                }
                if (e.code == "KeyE") {
                    BackLiftDown = false;
                }

                if (e.code == "Space") {
                    Close = false;
                }
                if (e.code == "KeyZ") {
                    Open = false;
                }
            });
            

        </script>
        <h2>Lehi TSA Robot UI</h2>
        <p>Use Key M to switch modes. Default is Automatic.</p>
        <p>Automatic Control:</p>
        <p>
            Keyboard Controls Key: <br>
            Forward, Backward, Turn Right, Turn Left - Arrow Keys <br>
            Slide Left - &lt; <br>
            Slide Right - &gt;<br>
            Lifts:<br>
            Front Expand - Q<br>
            Front Contract - 1<br>
            Middle Expand - W<br>
            Middle Contract - 2<br>
            Back Expand - E<br>
            Back Contract - 3<br>
            Pickup Mechanism:<br>
            Close - Space<br>
            Open - Z<br>
            Battery Balance:<br>
            Lean Forward - F<br>
            Lean Backward - B<br>

        </p>
        <p>Manual Control:</p>
        <div>
            <button onmousedown="var e = {code:'Digit1'}; KeyDown(e)" onmouseup="var e = {code:'Digit1'}; KeyUp(e)" onmouseout="var e = {code:'Digit1'}; KeyUp(e)">Lift1 Up (1)</button>
            <button onmousedown="var e = {code:'Digit2'}; KeyDown(e)" onmouseup="var e = {code:'Digit2'}; KeyUp(e)" onmouseout="var e = {code:'Digit2'}; KeyUp(e)">Lift2 Up (2)</button>
            <button onmousedown="var e = {code:'Digit3'}; KeyDown(e)" onmouseup="var e = {code:'Digit3'}; KeyUp(e)" onmouseout="var e = {code:'Digit3'}; KeyUp(e)">Lift3 Up (3)</button>
            <button onmousedown="var e = {code:'Digit4'}; KeyDown(e)" onmouseup="var e = {code:'Digit4'}; KeyUp(e)" onmouseout="var e = {code:'Digit4'}; KeyUp(e)">Motor4 Forward (4)</button>
            <button onmousedown="var e = {code:'Digit5'}; KeyDown(e)" onmouseup="var e = {code:'Digit5'}; KeyUp(e)" onmouseout="var e = {code:'Digit5'}; KeyUp(e)">Motor5 Forward (5)</button>
            <button onmousedown="var e = {code:'Digit6'}; KeyDown(e)" onmouseup="var e = {code:'Digit6'}; KeyUp(e)" onmouseout="var e = {code:'Digit6'}; KeyUp(e)">Motor6 Forward (6)</button>
            <button onmousedown="var e = {code:'Digit7'}; KeyDown(e)" onmouseup="var e = {code:'Digit7'}; KeyUp(e)" onmouseout="var e = {code:'Digit7'}; KeyUp(e)">Motor7 Forward (7)</button>
            <button onmousedown="var e = {code:'Digit8'}; KeyDown(e)" onmouseup="var e = {code:'Digit8'}; KeyUp(e)" onmouseout="var e = {code:'Digit8'}; KeyUp(e)">Pickup Open (8)</button>
            <button onmousedown="var e = {code:'Digit9'}; KeyDown(e)" onmouseup="var e = {code:'Digit9'}; KeyUp(e)" onmouseout="var e = {code:'Digit9'}; KeyUp(e)">Tilt Up (9)</button>
            <button onmousedown="var e = {code:'Digit0'}; KeyDown(e)" onmouseup="var e = {code:'Digit0'}; KeyUp(e)" onmouseout="var e = {code:'Digit0'}; KeyUp(e)">Balance Forward (0)</button>
        </div>
        <div>
            <button onmousedown="var e = {code:'KeyQ'}; KeyDown(e)" onmouseup="var e = {code:'KeyQ'}; KeyUp(e)" onmouseout="var e = {code:'KeyQ'}; KeyUp(e)">Lift1 Stop (q)</button>
            <button onmousedown="var e = {code:'KeyW'}; KeyDown(e)" onmouseup="var e = {code:'KeyW'}; KeyUp(e)" onmouseout="var e = {code:'KeyW'}; KeyUp(e)">Lift2 Stop (w)</button>
            <button onmousedown="var e = {code:'KeyE'}; KeyDown(e)" onmouseup="var e = {code:'KeyE'}; KeyUp(e)" onmouseout="var e = {code:'KeyE'}; KeyUp(e)">Lift3 Stop (e)</button>
            <button onmousedown="var e = {code:'KeyR'}; KeyDown(e)" onmouseup="var e = {code:'KeyR'}; KeyUp(e)" onmouseout="var e = {code:'KeyR'}; KeyUp(e)">Motor4 Stop (r)</button>
            <button onmousedown="var e = {code:'KeyT'}; KeyDown(e)" onmouseup="var e = {code:'KeyT'}; KeyUp(e)" onmouseout="var e = {code:'KeyT'}; KeyUp(e)">Motor5 Stop (t)</button>
            <button onmousedown="var e = {code:'KeyY'}; KeyDown(e)" onmouseup="var e = {code:'KeyY'}; KeyUp(e)" onmouseout="var e = {code:'KeyY'}; KeyUp(e)">Motor6 Stop (y)</button>
            <button onmousedown="var e = {code:'KeyU'}; KeyDown(e)" onmouseup="var e = {code:'KeyU'}; KeyUp(e)" onmouseout="var e = {code:'KeyU'}; KeyUp(e)">Motor7 Stop (u)</button>
            <button onmousedown="var e = {code:'KeyI'}; KeyDown(e)" onmouseup="var e = {code:'KeyI'}; KeyUp(e)" onmouseout="var e = {code:'KeyI'}; KeyUp(e)">Pickup Stop (i)</button>
            <button onmousedown="var e = {code:'KeyO'}; KeyDown(e)" onmouseup="var e = {code:'KeyO'}; KeyUp(e)" onmouseout="var e = {code:'KeyO'}; KeyUp(e)">Tilt Stop (o)</button>
            <button onmousedown="var e = {code:'KeyP'}; KeyDown(e)" onmouseup="var e = {code:'KeyP'}; KeyUp(e)" onmouseout="var e = {code:'KeyP'}; KeyUp(e)">Balance Stop (p)</button>
        </div>
        <div>
            <button onmousedown="var e = {code:'KeyA'}; KeyDown(e)" onmouseup="var e = {code:'KeyA'}; KeyUp(e)" onmouseout="var e = {code:'KeyA'}; KeyUp(e)">Lift1 Down (a)</button>
            <button onmousedown="var e = {code:'KeyS'}; KeyDown(e)" onmouseup="var e = {code:'KeyS'}; KeyUp(e)" onmouseout="var e = {code:'KeyS'}; KeyUp(e)">Lift2 Down (s)</button>
            <button onmousedown="var e = {code:'KeyD'}; KeyDown(e)" onmouseup="var e = {code:'KeyD'}; KeyUp(e)" onmouseout="var e = {code:'KeyD'}; KeyUp(e)">Lift3 Down (d)</button>
            <button onmousedown="var e = {code:'KeyF'}; KeyDown(e)" onmouseup="var e = {code:'KeyF'}; KeyUp(e)" onmouseout="var e = {code:'KeyF'}; KeyUp(e)">Motor4 Back (f)</button>
            <button onmousedown="var e = {code:'KeyG'}; KeyDown(e)" onmouseup="var e = {code:'KeyG'}; KeyUp(e)" onmouseout="var e = {code:'KeyG'}; KeyUp(e)">Motor5 Back (g)</button>
            <button onmousedown="var e = {code:'KeyH'}; KeyDown(e)" onmouseup="var e = {code:'KeyH'}; KeyUp(e)" onmouseout="var e = {code:'KeyH'}; KeyUp(e)">Motor6 Back (h)</button>
            <button onmousedown="var e = {code:'KeyJ'}; KeyDown(e)" onmouseup="var e = {code:'KeyJ'}; KeyUp(e)" onmouseout="var e = {code:'KeyJ'}; KeyUp(e)">Motor7 Back (j)</button>
            <button onmousedown="var e = {code:'KeyK'}; KeyDown(e)" onmouseup="var e = {code:'KeyK'}; KeyUp(e)" onmouseout="var e = {code:'KeyK'}; KeyUp(e)">Pickup Close (k)</button>
            <button onmousedown="var e = {code:'KeyL'}; KeyDown(e)" onmouseup="var e = {code:'KeyL'}; KeyUp(e)" onmouseout="var e = {code:'KeyL'}; KeyUp(e)">Tilt Down (l)</button>
            <button onmousedown="var e = {code:'Semicolon'}; KeyDown(e)" onmouseup="var e = {code:'Semicolon'}; KeyUp(e)" onmouseout="var e = {code:'Semicolon'}; KeyUp(e)">Balance Backward (;)</button>
        </div>
        
        
        
    </body>
</html>
)=====";