/**
 * @file joint_modules.hpp
 * @author Julian Viereck (jviereck@tuebingen.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-11-27
 *
 * @brief Joint module abstraction for
 */

#pragma once

#include <iostream>
#include <vector>
#include <unistd.h>

#include "master_board_sdk/defines.h"
#include "master_board_sdk/master_board_interface.h"

#include <odri_control_interface/common.hpp>

namespace odri_control_interface
{

/**
 * @brief Class abstracting the blmc motors to modules.
 */
class JointModules
{
protected:
    std::shared_ptr<MasterBoardInterface> robot_if_;
    std::vector<Motor*> motors_;

    Eigen::VectorXd gear_ratios_;
    Eigen::VectorXd motor_constants_;
    VectorXi polarities_;
    Eigen::VectorXd lower_joint_limits_;
    Eigen::VectorXd upper_joint_limits_;
    Eigen::VectorXd safety_damping_;

    // Cache for results.
    Eigen::VectorXd positions_;
    Eigen::VectorXd velocities_;
    Eigen::VectorXd sent_torques_;
    Eigen::VectorXd measured_torques_;

    // Cache for status bits.
    VectorXb index_been_detected_;
    VectorXb ready_;
    VectorXb enabled_;
    VectorXb motor_driver_enabled_;
    VectorXb motor_driver_errors_;

    Eigen::VectorXd zero_vector_;

    double max_joint_velocities_;

    int n_;
    int nd_;

    bool check_joint_limits_;

    std::ostream& msg_out_ = std::cout;

public:
    JointModules(
        const std::shared_ptr<MasterBoardInterface>& robot_if,
        ConstRefVectorXi motor_numbers,
        double motor_constants,
        double gear_ratios,
        double max_currents,
        ConstRefVectorXb reverse_polarities,
        ConstRefVectorXd lower_joint_limits,
        ConstRefVectorXd upper_joint_limits,
        double max_joint_velocities,
        double safety_damping
    );

    void Enable();

    void ParseSensorData();

    void SetTorques(ConstRefVectorXd desired_torques);
    void SetDesiredPositions(ConstRefVectorXd desired_positions);
    void SetDesiredVelocities(ConstRefVectorXd desired_velocities);
    void SetPositionGains(ConstRefVectorXd desired_gains);
    void SetVelocityGains(ConstRefVectorXd desired_gains);
    void SetMaximumCurrents(double max_currents);

    /**
     * @brief Disables the position and velocity gains by setting them to zero.
     */
    void SetZeroGains();
    void SetZeroCommands();

    /**
     * @brief Overwrites the control commands for a default safety controller.
     * The safety controller applies a D control to all the joints based
     * on the provided `safety_damping`.
     */
    virtual void RunSafetyController();

    // Used for calibration.
    void SetPositionOffsets(ConstRefVectorXd position_offsets);

    void EnableIndexOffsetCompensation();

    ConstRefVectorXb HasIndexBeenDetected();
    ConstRefVectorXb GetReady();
    ConstRefVectorXb GetEnabled();
    ConstRefVectorXb GetMotorDriverEnabled();
    ConstRefVectorXb GetMotorDriverErrors();

    bool SawAllIndices();

    /**
     * @brief Returns true once all motors are enabled and report ready.
     */
    bool IsReady();

    ConstRefVectorXd GetPositions();
    ConstRefVectorXd GetVelocities();
    ConstRefVectorXd GetSentTorques();
    ConstRefVectorXd GetMeasuredTorques();

    ConstRefVectorXd GetGearRatios();

    void DisableJointLimitCheck();
    void EnableJointLimitCheck();

    /**
     * @brief Checks for errors and prints them
     */
    bool HasError();

    void PrintVector(ConstRefVectorXd vector);

protected:
    int upper_joint_limits_counter_;
    int lower_joint_limits_counter_;
    int velocity_joint_limits_counter_;
    int motor_drivers_error_counter;
};

}  // namespace odri_control_interface
