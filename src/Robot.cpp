#include "WPILib.h"
#include "stdio.h"
class Robot: public IterativeRobot
{
private:
	LiveWindow *lw = LiveWindow::GetInstance();
	Encoder *leftEn;
	Encoder *rightEn;
	int leftRaw;
	int rightRaw;
	//int curStep;
	Servo *testServo;
	Servo *testServo2;
	bool buttonP;
	bool buttonP2;
	Joystick *stick2; //Joystick for the operator
	//Talon *uArm; //U-shaped arm
	Victor *inRMotor;
	Victor *inLMotor;
	RobotDrive *myRobot;
	Joystick *stick; //Joystick for the driver
	float axisX;
	float axisY;
	float deadDrive = 0.0;
	float deadOp = 0.0;
	Victor *armMotor;



	void RobotInit()
	{

		SmartDashboard::PutNumber("autonomous mode" ,0);
		SmartDashboard::PutNumber("autonomous Power %" ,55);
		//uArm = new Talon(2); //U-shaped arm
		inLMotor = new Victor(3);
		inRMotor = new Victor(2);
		leftEn = new Encoder(0,1,false,Encoder::k4X);
		rightEn = new Encoder(2,3,true,Encoder::k4X);
		myRobot = new RobotDrive(0,1);
		stick = new Joystick(0); //Joystick for the driver
		stick2 = new Joystick(1); //Joystick for the operator
		armMotor = new Victor(4);
		testServo = new Servo(5);
		testServo2 = new Servo(6);//change port
		buttonP = false;
		buttonP2 = false;

		CameraServer::GetInstance()->SetQuality(50);
		CameraServer::GetInstance()->StartAutomaticCapture("cam0"); //the camera name "cam0"
	}


	/**
	 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
	 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
	 * Dashboard, remove all of the chooser code and uncomment the GetString line to get the auto name from the text box
	 * below the Gyro
	 *
	 * You can add additional auto modes by adding additional comparisons to the if-else structure below with additional strings.
	 * If using the SendableChooser make sure to add them to the chooser code above as well.
	 */
	void AutonomousInit()
	{
		leftEn->Reset();
		rightEn->Reset();
		leftRaw = leftEn->GetRaw();
		rightRaw = rightEn->GetRaw();
	}

	void AutonomousPeriodic()
	{
		leftRaw = leftEn->GetRaw();
		rightRaw = -(rightEn->GetRaw());
		SmartDashboard::PutNumber("Left Encoder Value", leftRaw);
		SmartDashboard::PutNumber("Right Encoder Value", rightRaw);

		if (SmartDashboard::GetNumber("autonomous mode" ,0) == 0){
			//do nothing
		} else if (SmartDashboard::GetNumber("autonomous mode" ,0) == 1){
			if(leftRaw < 7520 and rightRaw < 7520){
				myRobot->ArcadeDrive(-0.01*(SmartDashboard::GetNumber("autonomous Power %", 55)), 0.0, false);
			}else{
				myRobot->ArcadeDrive(0.0, 0.0, false);
			}
		} else if (SmartDashboard::GetNumber("autonomous mode" ,0) == 2){
			if(leftRaw < 7520 and rightRaw < 7520){
				myRobot->ArcadeDrive(-0.01*(SmartDashboard::GetNumber("autonomous Power %", 55)), 0.0, false);
			}else{
				//myRobot->ArcadeDrive(0.0, 0.0, false);
				if (rightRaw < -7520+234){
					myRobot->ArcadeDrive(0.0, 0.45, false);
				}else{
					myRobot->ArcadeDrive(0.0, 0.0, false);
				}
			}
		}
	}

/*
 * These are the buttons and their ports
 * 1=A
 * 2=B
 * 3=X
 * 4=Y
 * 5=LB
 * 6=RB
 * 7=Back
 * 8=Start
 * 9=L-joy-push
 * 10=r-joy-push
 */
	void TeleopInit()
	{
		leftEn->Reset();
		rightEn->Reset();
		inLMotor->Set(0.1);// sets power for left side of arm
		inRMotor->Set(0.1);// sets power for right side of arm
	}

	void TeleopPeriodic()
	{
		leftRaw = leftEn->GetRaw();									//
		SmartDashboard::PutNumber("Left Encoder Value", leftRaw);	// Refresh the encoder
		rightRaw = -(rightEn->GetRaw());							//
		SmartDashboard::PutNumber("Right Encoder Value", rightRaw);	//

		//----------------------------Drive Train--------------------------------------------
		axisX = -0.5*((stick->GetRawAxis(0))/* stick->GetRawAxis(0)*/);
		axisY = 0.8*(stick->GetRawAxis(1));

		if((axisX <= deadDrive && axisX > 0) || (axisX >= -1*deadDrive && axisX <0)){
			axisX = 0.0; //Sets deadband for axisX
		}
		if((axisY <= deadDrive && axisY > 0) || (axisY >= -1*deadDrive && axisY <0)){
			axisY = 0.0; //Sets deadband for axisY
		}
		myRobot->ArcadeDrive(axisY, axisX, false); // drive with arcade style (use right stick)

		//--------------------------------Redundant------------------------------------------

		if (stick2->GetRawButton(1)){ // moves arm up (A button on X-box)
			inLMotor->Set(-1.0);
			inRMotor->Set(1.0);
		} else {
			inLMotor->Set(0.0);
			inRMotor->Set(0.0);
		}

		//---------------------------------Paddle control------------------------------------

		if (stick2->GetRawAxis(1)> deadOp){
			armMotor->Set(0.60* stick2->GetRawAxis(1)); // moves arm up and down
		} else if(stick2->GetRawAxis(1)< -(deadOp)) {
			armMotor->Set(0.60* stick2->GetRawAxis(1)); // moves arm up and down
		} else {
			armMotor->Set(0.0); // stops and resets arm motor
		}

		if (stick2->GetRawButton(2)){
			testServo->SetAngle(85);
			testServo2->SetAngle(85);
		} else if (stick2->GetRawButton(3)){
			testServo->SetAngle(0);
			testServo->SetAngle(0);
		} else {
			//leave this blank (buttons are X and B)
		}

		/*if(stick2->GetRawButton(6) and !(buttonP)){ //compares present state of RB to the previous state of RB
			armMotor->Set(-0.9);//sets the speed of the arm to 70 percent
			Wait(0.15);//holds program f0r 1/10 of a sec
			armMotor->Set(0.0);//stops motor
		}
		buttonP = stick2->GetRawButton(6);//sets past state to the present state

		if(stick2->GetRawButton(5) and !(buttonP2)){
			armMotor->Set(-0.9);
			Wait(0.25);
			armMotor->Set(0.0);
		}
		buttonP2 = stick2->GetRawButton(4);*/

	}

	void TestPeriodic()
	{
		lw->Run();
	}
};

START_ROBOT_CLASS(Robot)
