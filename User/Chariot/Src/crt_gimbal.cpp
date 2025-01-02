/**
 * @file crt_gimbal.cpp
 * @author lez by wanghongxi
 * @brief 云台
 * @version 0.1
 * @date 2024-07-1 0.1 24赛季定稿
 *
 * @copyright ZLLC 2024
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "crt_gimbal.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function declarations ---------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

float NoiseFilter(float current_value, float last_value, float threshold)
{
    // 计算变化值
    float delta = current_value - last_value;

    // 如果变化值小于噪声阈值，返回上次值
    if (fabs(delta) <= threshold)
    {
        return last_value;
    }

    // 否则，返回当前值
    return current_value;
}


/**
 * @brief TIM定时器中断计算回调函数
 *
 */
float Test_Target_Omega=0;
void Class_Gimbal_Yaw_Motor_GM6020::TIM_PID_PeriodElapsedCallback()
{
    switch (DJI_Motor_Control_Method)
    {
    case (DJI_Motor_Control_Method_OPENLOOP):
    {
        //默认开环速度控制
        Set_Out(Target_Omega_Angle / Omega_Max * Output_Max);
    }
    break;
    case (DJI_Motor_Control_Method_TORQUE):
    {
        //力矩环
        PID_Torque.Set_Target(Target_Torque);
        PID_Torque.Set_Now(Data.Now_Torque);
        PID_Torque.TIM_Adjust_PeriodElapsedCallback();

        Set_Out(PID_Torque.Get_Out());
    }
    break;
    case (DJI_Motor_Control_Method_IMU_OMEGA):
    {
        //角速度环
        PID_Omega.Set_Target(Target_Omega_Angle);
        if (IMU->Get_IMU_Status()==IMU_Status_DISABLE)
        {
            PID_Omega.Set_Now(Data.Now_Omega_Angle);
        }
        else
        {
            PID_Omega.Set_Now(True_Gyro_Yaw*180.f/PI);
        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();

        Target_Torque = PID_Omega.Get_Out();
        Set_Out(PID_Omega.Get_Out());
    }
    break;
    case (DJI_Motor_Control_Method_IMU_ANGLE):
    {
        PID_Angle.Set_Target(Target_Angle);
        if (IMU->Get_IMU_Status()!=IMU_Status_DISABLE)
        {
            //角度环
            PID_Angle.Set_Now(True_Angle_Yaw);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Radian = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Radian);
            PID_Omega.Set_Now(True_Gyro_Yaw);

        }
        else
        {
            //角度环
            PID_Angle.Set_Now(Data.Now_Angle);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Angle = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Angle);
            PID_Omega.Set_Now(Data.Now_Omega_Angle);

        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();

        Target_Torque = PID_Omega.Get_Out();
        Set_Out(PID_Omega.Get_Out());
    }
    break;
    default:
    {
        Set_Out(0.0f);
    }
    break;
    }
    Output();
}

/**
 * @brief 根据不同c板的放置方式来修改这个函数
 *
 */
void Class_Gimbal_Yaw_Motor_GM6020::Transform_Angle()
{
    True_Rad_Yaw = IMU->Get_Rad_Yaw();
    True_Gyro_Yaw = IMU->Get_Gyro_Yaw(); 
    True_Angle_Yaw = IMU->Get_Angle_Yaw();   
}


/**
 * @brief TIM定时器中断计算回调函数
 *
 */
void Class_Gimbal_Pitch_Motor_GM6020::TIM_PID_PeriodElapsedCallback()
{
    switch (DJI_Motor_Control_Method)
    {
    case (DJI_Motor_Control_Method_OPENLOOP):
    {
        //默认开环速度控制
        Set_Out(Target_Omega_Angle / Omega_Max * Output_Max);
    }
    break;
    case (DJI_Motor_Control_Method_TORQUE):
    {
        //力矩环
        PID_Torque.Set_Target(Target_Torque);
        PID_Torque.Set_Now(Data.Now_Torque);
        PID_Torque.TIM_Adjust_PeriodElapsedCallback();

        Set_Out(PID_Torque.Get_Out());
    }
    break;
    case (DJI_Motor_Control_Method_IMU_OMEGA):
    {
        //角速度环
        PID_Omega.Set_Target(Target_Omega_Angle);
        if (IMU->Get_IMU_Status()==IMU_Status_DISABLE)
        {
            PID_Omega.Set_Now(Data.Now_Omega_Angle);
        }
        else
        {
            PID_Omega.Set_Now(True_Gyro_Pitch*180.f/PI);
        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();

        Target_Torque = PID_Omega.Get_Out();
        Set_Out(PID_Omega.Get_Out());
    }
    break;
    case (DJI_Motor_Control_Method_IMU_ANGLE):
    {
        PID_Angle.Set_Target(Target_Angle);

        if (IMU->Get_IMU_Status()!=IMU_Status_DISABLE)
        {

            //角度环
            PID_Angle.Set_Now(True_Rad_Pitch);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Angle = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Angle);
            PID_Omega.Set_Now(True_Gyro_Pitch*180.f/PI);

        }
        else
        {
            //角度环
            PID_Angle.Set_Now(Data.Now_Angle);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Angle = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Angle);
            PID_Omega.Set_Now(Data.Now_Omega_Angle);

        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();

        Target_Torque = PID_Omega.Get_Out();
        Set_Out(PID_Omega.Get_Out() + Gravity_Compensate);
    }
    break;
    default:
    {
        Set_Out(0.0f);
    }
    break;
    }
    Output();
}

/**
 * @brief 根据不同c板的放置方式来修改这个函数
 *
 */
void Class_Gimbal_Pitch_Motor_GM6020::Transform_Angle()
{
    True_Rad_Pitch = -1 * IMU->Get_Rad_Pitch();
    True_Gyro_Pitch = -1 * IMU->Get_Gyro_Pitch(); 
    True_Angle_Pitch = -1 * IMU->Get_Angle_Pitch();   
}

/**
 * @brief TIM定时器中断计算回调函数
 *
 */
void Class_Gimbal_Pitch_Motor_LK6010::TIM_PID_PeriodElapsedCallback()
{
    switch (LK_Motor_Control_Method)
    {
    case (LK_Motor_Control_Method_TORQUE):
    {
        Out = Target_Torque*Torque_Current/Current_Max*Current_Max_Cmd;
        Set_Out(Out);
    }
    break;
    case (LK_Motor_Control_Method_IMU_OMEGA):
    {
        //角速度环
        PID_Omega.Set_Target(Target_Omega_Angle);
        if (IMU->Get_IMU_Status()==IMU_Status_DISABLE)
        {
            PID_Omega.Set_Now(Data.Now_Omega_Angle);
        }
        else
        {
            PID_Omega.Set_Now(True_Gyro_Pitch*180.f/PI);
        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();
        Out = PID_Omega.Get_Out();
        Set_Out(Out);
    }
    break;
    case (LK_Motor_Control_Method_IMU_ANGLE):
    {
        PID_Angle.Set_Target(Target_Angle);
        if (IMU->Get_IMU_Status()!=IMU_Status_DISABLE)
        {
            //角度环
            PID_Angle.Set_Now(True_Angle_Pitch);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Angle = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Angle);
            PID_Omega.Set_Now(True_Gyro_Pitch*180.f/PI);
        }
        else
        {
            //角度环
            PID_Angle.Set_Now(Data.Now_Angle);
            PID_Angle.TIM_Adjust_PeriodElapsedCallback();

            Target_Omega_Angle = PID_Angle.Get_Out();

            //速度环
            PID_Omega.Set_Target(Target_Omega_Angle);
            PID_Omega.Set_Now(Data.Now_Omega_Angle);

        }
        PID_Omega.TIM_Adjust_PeriodElapsedCallback();

        Out = PID_Omega.Get_Out() + Gravity_Compensate;
        Set_Out(Out);
    }
    break;
    default:
    {
        Set_Out(0.0f);
    }
    break;
    }
    Output();   
}

/**
 * @brief 根据不同c板的放置方式来修改这个函数
 *
 */
void Class_Gimbal_Pitch_Motor_LK6010::Transform_Angle()
{
    True_Rad_Pitch = 1 * IMU->Get_Rad_Pitch();
    True_Gyro_Pitch = 1 * IMU->Get_Gyro_Pitch(); 
    True_Angle_Pitch = 1 * IMU->Get_Angle_Pitch();
}

/**
 * @brief 云台初始化
 *
 */
void Class_Gimbal::Init()
{
    //imu初始化
    Boardc_BMI.Init(); 

    //yaw轴电机
    Motor_Yaw_A.PID_Angle.Init(0.1f, 0.0f, 0.0f, 0.0f, 3, 15,0.0f,0.0f,0,0.001f,0.0f);
    Motor_Yaw_A.PID_Omega.Init(10.0f, 5.0f, 0.0f, 0.0f, 7000.0f, 20000.0f,0.0f,0.0f,0.0f,0.001f,0.0f);
    Motor_Yaw_A.PID_Torque.Init(0.78f, 100.0f, 0.0f, 0.0f, Motor_Yaw_A.Get_Output_Max(), Motor_Yaw_A.Get_Output_Max());
    Motor_Yaw_A.Init(&hfdcan2, DJI_Motor_ID_0x207, DJI_Motor_Control_Method_ANGLE, 2048);

    Motor_Yaw_B.PID_Angle.Init(0.1f, 0.0f, 0.0f, 0.0f, 3, 15,0.0f,0.0f,0,0.001f,0.0f);
    Motor_Yaw_B.PID_Omega.Init(10.0f, 5.0f, 0.0f, 0.0f, 7000.0f, 20000.0f,0.0f,0.0f,0.0f,0.001f,0.0f);
    Motor_Yaw_B.PID_Torque.Init(0.78f, 100.0f, 0.0f, 0.0f, Motor_Yaw_B.Get_Output_Max(), Motor_Yaw_B.Get_Output_Max());
    Motor_Yaw_B.Init(&hfdcan2, DJI_Motor_ID_0x205, DJI_Motor_Control_Method_ANGLE, 2048);
    
    Motor_Main_Yaw.PID_Angle.Init(0.5f, 0.0f, 0.0f, 0.0f, 3, 15,0.0f,0.0f,0,0.001f,0.0f);
    Motor_Main_Yaw.PID_Omega.Init(1000.0f, 5.0f, 0.0f, 0.0f, 100.0f, 2048.0f,0.0f,0.0f,0.0f,0.001f,0.0f);
    Motor_Main_Yaw.PID_Torque.Init(0.78f, 100.0f, 0.0f, 0.0f, Motor_Main_Yaw.Get_Output_Max(), Motor_Main_Yaw.Get_Output_Max());
    Motor_Main_Yaw.Init(&hfdcan3, LK_Motor_ID_0x141, LK_Motor_Control_Method_ANGLE, 2048);

    //pitch轴电机
    Motor_Pitch_A.PID_Angle.Init(0.1f, 0.0f, 0.1f, 0.0f, 6.0f * PI, 6.0f * PI);
    Motor_Pitch_A.PID_Omega.Init(10.0f, 5.0f, 0.0f, 0, Motor_Pitch_A.Get_Output_Max(), Motor_Pitch_A.Get_Output_Max(),0.0f,0.0f,0.0f,0.001f,0.8f);
    Motor_Pitch_A.PID_Torque.Init(0.8f, 100.0f, 0.0f, 0.0f, Motor_Pitch_A.Get_Output_Max(), Motor_Pitch_A.Get_Output_Max());
    Motor_Pitch_A.Init(&hfdcan2, DJI_Motor_ID_0x206, DJI_Motor_Control_Method_ANGLE, 3413);

    Motor_Pitch_B.PID_Angle.Init(0.1f, 0.0f, 0.1f, 0.0f, 6.0f * PI, 6.0f * PI);
    Motor_Pitch_B.PID_Omega.Init(10.0f, 5.0f, 0.0f, 0, Motor_Pitch_B.Get_Output_Max(), Motor_Pitch_B.Get_Output_Max(),0.0f,0.0f,0.0f,0.001f,0.8f);
    Motor_Pitch_B.PID_Torque.Init(0.8f, 100.0f, 0.0f, 0.0f, Motor_Pitch_B.Get_Output_Max(), Motor_Pitch_B.Get_Output_Max());
    Motor_Pitch_B.Init(&hfdcan2, DJI_Motor_ID_0x208, DJI_Motor_Control_Method_ANGLE, 3413);

    Motor_Main_Yaw.Set_Zero_Position(145.1316f);
    Motor_Yaw_A.Set_Zero_Position(294.9169f);
    Motor_Yaw_B.Set_Zero_Position(88.9013f);
    Motor_Pitch_A.Set_Zero_Position(47.4169f);
    Motor_Pitch_B.Set_Zero_Position(20.0830f);

}

/**
 * @brief 输出到电机
 *
 */
void Class_Gimbal::Output()
{
    if (Gimbal_Control_Type == Gimbal_Control_Type_DISABLE)
    {
        //云台失能
        Motor_Yaw_A.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_TORQUE);
        Motor_Yaw_B.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_TORQUE);
        Motor_Main_Yaw.Set_LK_Motor_Control_Method(LK_Motor_Control_Method_TORQUE);
        Motor_Pitch_A.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_TORQUE);
        Motor_Pitch_B.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_TORQUE);

        Motor_Yaw_A.PID_Angle.Set_Integral_Error(0.0f);
        Motor_Yaw_A.PID_Omega.Set_Integral_Error(0.0f);
        Motor_Yaw_A.PID_Torque.Set_Integral_Error(0.0f);
        Motor_Yaw_B.PID_Angle.Set_Integral_Error(0.0f);
        Motor_Yaw_B.PID_Omega.Set_Integral_Error(0.0f);
        Motor_Yaw_B.PID_Torque.Set_Integral_Error(0.0f);
        Motor_Main_Yaw.PID_Angle.Set_Integral_Error(0.0f);
        Motor_Main_Yaw.PID_Omega.Set_Integral_Error(0.0f);
        Motor_Main_Yaw.PID_Torque.Set_Integral_Error(0.0f);
        Motor_Pitch_A.PID_Angle.Set_Integral_Error(0.0f);
        Motor_Pitch_A.PID_Omega.Set_Integral_Error(0.0f);
        Motor_Pitch_A.PID_Torque.Set_Integral_Error(0.0f);
        Motor_Pitch_B.PID_Angle.Set_Integral_Error(0.0f);
        Motor_Pitch_B.PID_Omega.Set_Integral_Error(0.0f);
        Motor_Pitch_B.PID_Torque.Set_Integral_Error(0.0f);

        Motor_Yaw_A.Set_Target_Torque(0.0f);
        Motor_Yaw_B.Set_Target_Torque(0.0f);
        Motor_Main_Yaw.Set_Target_Torque(0.0f);
        Motor_Pitch_A.Set_Target_Torque(0.0f);
        Motor_Pitch_B.Set_Target_Torque(0.0f);

        Motor_Main_Yaw.Set_Out(0.0f);
        Motor_Yaw_A.Set_Out(0.0f);
        Motor_Yaw_B.Set_Out(0.0f);
        Motor_Pitch_A.Set_Out(0.0f);
        Motor_Pitch_B.Set_Out(0.0f);

    }
    else // 非失能模式
    {          
        if (Gimbal_Control_Type == Gimbal_Control_Type_NORMAL)
        {
            Motor_Main_Yaw.Set_LK_Motor_Control_Method(LK_Motor_Control_Method_ANGLE);
            Motor_Yaw_A.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_ANGLE);
            Motor_Yaw_B.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_ANGLE);
            Motor_Pitch_A.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_ANGLE);
            Motor_Pitch_B.Set_DJI_Motor_Control_Method(DJI_Motor_Control_Method_ANGLE);

            //设置目标角度
            Motor_Yaw_A.Set_Target_Angle(0.0f);
            Motor_Yaw_B.Set_Target_Angle(0.0f);
            Motor_Main_Yaw.Set_Target_Angle(Target_Yaw_Angle);
            Motor_Pitch_A.Set_Target_Angle(0.0f);
            Motor_Pitch_B.Set_Target_Angle(0.0f);
                       
        }
        // else if((Gimbal_Control_Type == Gimbal_Control_Type_MINIPC) && (MiniPC->Get_MiniPC_Status()!=MiniPC_Status_DISABLE))
        // {   
        //     Target_Pitch_Angle = MiniPC->Get_Rx_Pitch_Angle();
        //     Target_Yaw_Angle = MiniPC->Get_Rx_Yaw_Angle();          
        // }

        // //限制角度范围 处理yaw轴180度问题
        // if((Target_Yaw_Angle-Motor_Yaw.Get_True_Angle_Yaw())>Max_Yaw_Angle)
        // {
        //     Target_Yaw_Angle -= (2 * Max_Yaw_Angle);
        // }
        // else if((Target_Yaw_Angle-Motor_Yaw.Get_True_Angle_Yaw())<-Max_Yaw_Angle)
        // {
        //     Target_Yaw_Angle += (2 * Max_Yaw_Angle);
        // }
    
        // //pitch限位
        // Math_Constrain(&Target_Pitch_Angle, Min_Pitch_Angle, Max_Pitch_Angle);

        // //设置目标角度
        // Motor_Yaw.Set_Target_Angle(Target_Yaw_Angle);
        // Motor_Pitch.Set_Target_Angle(Target_Pitch_Angle);
        // Motor_Pitch_LK6010.Set_Target_Angle(Target_Pitch_Angle); 
    }
}


/**
 * @brief TIM定时器中断计算回调函数
 *
 */
void Class_Gimbal::TIM_Calculate_PeriodElapsedCallback()
{
    Output();
    
    //根据零位的不同来修改这几个函数
    Yaw_Angle_Transform_A();
    Yaw_Angle_Transform_B();
    Pitch_Angle_Transform_A();  
    Pitch_Angle_Transform_B();
    Yaw_Angle_Transform_Main();

    Motor_Yaw_A.TIM_PID_PeriodElapsedCallback();
    Motor_Yaw_B.TIM_PID_PeriodElapsedCallback();
    Motor_Main_Yaw.TIM_Process_PeriodElapsedCallback();
    Motor_Pitch_A.TIM_PID_PeriodElapsedCallback();
    Motor_Pitch_B.TIM_PID_PeriodElapsedCallback();
}

void Class_Gimbal::Yaw_Angle_Transform_Main()
{
    //LK电机正方向未设置 不一定有效 6020有效
    if(Motor_Main_Yaw.Get_Now_Angle() > Motor_Main_Yaw.Get_Zero_Position()){
        Set_True_Angle_Yaw_Main(-(Motor_Main_Yaw.Get_Now_Angle() - Motor_Main_Yaw.Get_Zero_Position()));//电机数据转现实坐标系
        if(Get_True_Angle_Yaw_Main()< -180.0f) Set_True_Angle_Yaw_Main(Get_True_Angle_Yaw_Main() + 360.0f);
    }
    else if (Motor_Main_Yaw.Get_Now_Angle() < Motor_Main_Yaw.Get_Zero_Position())
    {
        Set_True_Angle_Yaw_Main( Motor_Main_Yaw.Get_Zero_Position() - Motor_Main_Yaw.Get_Now_Angle());
        if(Get_True_Angle_Yaw_Main() > 180.0f) Set_True_Angle_Yaw_Main(Get_True_Angle_Yaw_Main() - 360.0f);
    }
    Set_True_Angle_Yaw_Main(-Get_True_Angle_Yaw_Main());
    Control_Update_Main();

}
void Class_Gimbal::Yaw_Angle_Transform_A()
{
    if(Motor_Yaw_A.Get_Now_Angle() > Motor_Yaw_A.Get_Zero_Position()){
        Set_True_Angle_Yaw_A(-(Motor_Yaw_A.Get_Now_Angle() - Motor_Yaw_A.Get_Zero_Position()));//电机数据转现实坐标系
        if(Get_True_Angle_Yaw_A()< -180.0f) Set_True_Angle_Yaw_A(Get_True_Angle_Yaw_A() + 360.0f);
    }
    else if (Motor_Yaw_A.Get_Now_Angle() < Motor_Yaw_A.Get_Zero_Position())
    {
        Set_True_Angle_Yaw_A( Motor_Yaw_A.Get_Zero_Position() - Motor_Yaw_A.Get_Now_Angle());
        if(Get_True_Angle_Yaw_A() > 180.0f) Set_True_Angle_Yaw_A(Get_True_Angle_Yaw_A() - 360.0f);
    }
    Set_True_Angle_Yaw_A(-Get_True_Angle_Yaw_A());
    Motor_Yaw_A.Set_Transform_Angle(Get_True_Angle_Yaw_A());
    Motor_Yaw_A.Set_Transform_Omega(Motor_Yaw_A.Get_Now_Omega_Angle());
    Motor_Yaw_A.Set_Transform_Torque(-Motor_Yaw_A.Get_Now_Torque());
}
void Class_Gimbal::Yaw_Angle_Transform_B()
{
    if(Motor_Yaw_B.Get_Now_Angle() > Motor_Yaw_B.Get_Zero_Position()){
        Set_True_Angle_Yaw_B(-(Motor_Yaw_B.Get_Now_Angle() - Motor_Yaw_B.Get_Zero_Position()));//电机数据转现实坐标系
        if(Get_True_Angle_Yaw_B()< -180.0f) Set_True_Angle_Yaw_B(Get_True_Angle_Yaw_B() + 360.0f);
    }
    else if (Motor_Yaw_B.Get_Now_Angle() < Motor_Yaw_B.Get_Zero_Position())
    {
        Set_True_Angle_Yaw_B( Motor_Yaw_B.Get_Zero_Position() - Motor_Yaw_B.Get_Now_Angle());
        if(Get_True_Angle_Yaw_B() > 180.0f) Set_True_Angle_Yaw_B(Get_True_Angle_Yaw_B() - 360.0f);
    }
    Set_True_Angle_Yaw_B(-Get_True_Angle_Yaw_B());
    Motor_Yaw_B.Set_Transform_Angle(Get_True_Angle_Yaw_B());
    Motor_Yaw_B.Set_Transform_Omega(Motor_Yaw_B.Get_Now_Omega_Angle());
    Motor_Yaw_B.Set_Transform_Torque(-Motor_Yaw_B.Get_Now_Torque());
}
void Class_Gimbal::Pitch_Angle_Transform_A()
{
    if(Motor_Pitch_A.Get_Now_Angle() > Motor_Pitch_A.Get_Zero_Position()){
        Set_True_Angle_Pitch_A(-(Motor_Pitch_A.Get_Now_Angle() - Motor_Pitch_A.Get_Zero_Position()));//电机数据转现实坐标系
        if(Get_True_Angle_Pitch_A()< -180.0f) Set_True_Angle_Pitch_A(Get_True_Angle_Pitch_A() + 360.0f);
    }
    else if (Motor_Pitch_A.Get_Now_Angle() < Motor_Pitch_A.Get_Zero_Position())
    {
        Set_True_Angle_Pitch_A( Motor_Pitch_A.Get_Zero_Position() - Motor_Pitch_A.Get_Now_Angle());
        if(Get_True_Angle_Pitch_A() > 180.0f) Set_True_Angle_Pitch_A(Get_True_Angle_Pitch_A() - 360.0f);
    }
    Motor_Pitch_A.Set_Transform_Angle(Get_True_Angle_Pitch_A());
    Motor_Pitch_A.Set_Transform_Omega(-Motor_Pitch_A.Get_Now_Omega_Angle());
    Motor_Pitch_A.Set_Transform_Torque(Motor_Pitch_A.Get_Now_Torque());
}
void Class_Gimbal::Pitch_Angle_Transform_B()
{
    if(Motor_Pitch_B.Get_Now_Angle() > Motor_Pitch_B.Get_Zero_Position()){
        Set_True_Angle_Pitch_B(-(Motor_Pitch_B.Get_Now_Angle() - Motor_Pitch_B.Get_Zero_Position()));//电机数据转现实坐标系
        if(Get_True_Angle_Pitch_B()< -180.0f) Set_True_Angle_Pitch_B(Get_True_Angle_Pitch_B() + 360.0f);
    }
    else if (Motor_Pitch_B.Get_Now_Angle() < Motor_Pitch_B.Get_Zero_Position())
    {
        Set_True_Angle_Pitch_B( Motor_Pitch_B.Get_Zero_Position() - Motor_Pitch_B.Get_Now_Angle());
        if(Get_True_Angle_Pitch_B() > 180.0f) Set_True_Angle_Pitch_B(Get_True_Angle_Pitch_B() - 360.0f);
    }
    Set_True_Angle_Pitch_B(-Get_True_Angle_Pitch_B());
    Motor_Pitch_B.Set_Transform_Angle(Get_True_Angle_Pitch_B());
    Motor_Pitch_B.Set_Transform_Omega(Motor_Pitch_B.Get_Now_Omega_Angle());
    Motor_Pitch_B.Set_Transform_Torque(-Motor_Pitch_B.Get_Now_Torque());
}

void Class_Gimbal::Control_Update_Main()
{
    int invert_flag = 0;
    float temp_error = 0,temp_min = 0,pre_angle = 0,pre_omega = 0;
    temp_error = Get_Target_Yaw_Angle() - Boardc_BMI.Get_Angle_Yaw() - invert_flag*180;
    while (temp_error > 360.0f)
        temp_error -= 360.0f;
    while (temp_error < 0.0f)
        temp_error += 360.0f;
    if (fabs(temp_error) < (360.0f - fabs(temp_error)))
        temp_min = fabs(temp_error);
    else
        temp_min = 360.0f - fabs(temp_error);
    if (temp_min > 90.0f)
    {
        invert_flag = !invert_flag;
        // 重新计算误差
        temp_error = Get_Target_Yaw_Angle() - Boardc_BMI.Get_Angle_Yaw() - invert_flag * 180.0f;
    }

    if(temp_error > 180.0f) temp_error -= 360.0f;
    else if(temp_error < -180.0f) temp_error += 360.0f;

    Set_Target_Yaw_Angle(Boardc_BMI.Get_Angle_Yaw() + temp_error);
    Motor_Main_Yaw.Set_Transform_Angle(NoiseFilter(Boardc_BMI.Get_Angle_Yaw(),pre_angle,0.5));
    Motor_Main_Yaw.Set_Transform_Omega(NoiseFilter(Boardc_BMI.Get_Gyro_Yaw(),pre_omega,0.2));

    pre_angle = Boardc_BMI.Get_Angle_Yaw();
    pre_omega = Boardc_BMI.Get_Gyro_Yaw();

}

void Class_Gimbal::Control_Update(Class_DJI_Motor_GM6020 *Motor)
{
    
}
/************************ COPYRIGHT(C) USTC-ROBOWALKER **************************/
