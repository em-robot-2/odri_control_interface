/**
 * @file joint_modules.hpp
 * @author Julian Viereck (jviereck@tuebingen.mpg.de)
 * license License BSD-3-Clause
 * @copyright Copyright (c) 2020, New York University and Max Planck
 * Gesellschaft.
 * @date 2020-12-04
 *
 * @brief Python bindings to the library.
 */


#include "bindings.h"
#include <eigenpy/eigenpy.hpp>

using namespace boost::python;
using namespace odri_control_interface;

typedef Eigen::Matrix<long, Eigen::Dynamic, 1> VectorXl;
typedef Eigen::Matrix<bool, Eigen::Dynamic, 1> VectorXb;

JointCalibrator* joint_calibrator_constructor(
    JointModules* joints,
    boost::python::list search_methods,
    RefVectorXd position_offsets,
    double Kp, double Kd, double T, double dt
)
{
    boost::python::ssize_t len = boost::python::len(search_methods);
    std::vector<CalibrationMethod> search_method_vec;
    for(int i = 0; i < len; i++)
    {
        search_method_vec.push_back(
            boost::python::extract<CalibrationMethod>(search_methods[i])
        );
    }
    return new JointCalibrator(
        joints, search_method_vec, position_offsets, Kp, Kd, T, dt
    );
}

BOOST_PYTHON_MODULE(libodri_control_interface_pywrap)
{
    eigenpy::enableEigenPy();
    eigenpy::enableEigenPySpecific<VectorXl>();
    eigenpy::enableEigenPySpecific<VectorXb >();

    class_<JointModules>("JointModules", init<
            MasterBoardInterface*,
            RefVectorXl, double, double, double, RefVectorXb,
            RefVectorXd, RefVectorXd,
            double, double>())

        .def("enable", &JointModules::Enable)

        .def("set_torques", &JointModules::SetTorques)
        .def("set_desired_positions", &JointModules::SetDesiredPositions)
        .def("set_desired_velocities", &JointModules::SetDesiredVelocities)
        .def("set_position_gains", &JointModules::SetPositionGains)
        .def("set_velocity_gains", &JointModules::SetVelocityGains)

        .def("set_zero_gains", &JointModules::SetZeroGains)
        .def("set_zero_commands", &JointModules::SetZeroCommands)

        .def("set_position_offsets", &JointModules::SetPositionOffsets)
        .def("enable_index_offset_compensation", &JointModules::EnableIndexOffsetCompensation)
        .def("set_maximum_current", &JointModules::SetMaximumCurrents)

        .def("disable_joint_limit_check", &JointModules::DisableJointLimitCheck)
        .def("enable_joint_limit_check", &JointModules::EnableJointLimitCheck)

        .add_property("ready", &JointModules::GetReady)
        .add_property("enabled", &JointModules::GetEnabled)

        .add_property("saw_all_indices", &JointModules::SawAllIndices)
        .add_property("is_ready", &JointModules::IsReady)
        .add_property("has_error", &JointModules::HasError)

        .add_property("positions", &JointModules::GetPositions)
        .add_property("velocities", &JointModules::GetVelocities)
        .add_property("sent_torques", &JointModules::GetSentTorques)
        .add_property("measured_torques", &JointModules::GetMeasuredTorques)

        .add_property("gear_ratios", &JointModules::GetGearRatios);

    class_<IMU>("IMU", init<MasterBoardInterface*>())
        .def(init<MasterBoardInterface*, RefVectorXl, RefVectorXl>())

        .add_property("has_error", &IMU::HasError)

        .add_property("gyroscope", &IMU::GetGyroscope)
        .add_property("accelerometer", &IMU::GetAccelerometer)
        .add_property("linear_acceleration", &IMU::GetLinearAcceleration)
        .add_property("attitude_euler", &IMU::GetAttitudeEuler)
        .add_property("attitude_quaternion", &IMU::GetAttitudeQuaternion);

    class_<Robot>("Robot", init<MasterBoardInterface*, JointModules*, IMU*>())
        .def("init", &Robot::Init)
        .def("sendInit", &Robot::SendInit)
        .def("start", &Robot::Start)
        .def("wait_until_ready", &Robot::WaitUntilReady)

        .def("parse_sensor_data", &Robot::ParseSensorData)
        .def("send_command", &Robot::SendCommand)
        .def("send_command_and_wait_end_of_cycle", &Robot::SendCommandAndWaitEndOfCycle)

        .def("run_calibration", &Robot::RunCalibration)

        .def("report_error", &Robot::ReportError)

        .add_property("robot_interface", make_function(&Robot::GetRobotInterface,
                    return_value_policy<boost::python::reference_existing_object>()))
        .add_property("joints", make_function(&Robot::GetJoints,
                    return_value_policy<boost::python::reference_existing_object>()))
        .add_property("imu", make_function(&Robot::GetIMU,
                return_value_policy<reference_existing_object>()))

        .add_property("is_ready", &Robot::IsReady)
        .add_property("is_timeout", &Robot::IsTimeout)
        .add_property("is_ack_msg_received", &Robot::IsAckMsgReceived)
        .add_property("has_error", &Robot::HasError);

    enum_<CalibrationMethod>("CalibrationMethod")
        .value("auto", CalibrationMethod::AUTO)
        .value("positive", CalibrationMethod::POSITIVE)
        .value("negative", CalibrationMethod::NEGATIVE)
        .value("alternative", CalibrationMethod::ALTERNATIVE)
        ;

    class_<JointCalibrator>("JointCalibrator", no_init)
        .def("__init__", make_constructor(&joint_calibrator_constructor))
        .def("run", &JointCalibrator::Run)
        ;

    def("robot_from_yaml_file", make_function(
            &RobotFromYamlFile, return_value_policy<reference_existing_object>()));
    def("joint_calibrator_from_yaml_file", make_function(
            &JointCalibratorFromYamlFile, return_value_policy<reference_existing_object>()));

}
