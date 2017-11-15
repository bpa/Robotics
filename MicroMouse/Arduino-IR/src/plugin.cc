#ifndef MICROMOUSE_PLUGIN
#define MICROMOUSE_PLUGIN

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>

namespace gazebo
{
  class MicromousePlugin : public ModelPlugin
  {
    public: MicromousePlugin() {}

    public: virtual void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf)
    {
      this->model = _model;

      this->node = transport::NodePtr(new transport::Node());
      #if GAZEBO_MAJOR_VERSION < 8
      this->node->Init(this->model->GetWorld()->GetName());
      #else
      this->node->Init(this->model->GetWorld()->Name());
      #endif

      std::string topicName = "~/" + this->model->GetName() + "/odom";

      //this->sub = this->node->Subscribe(topicName,
         //&MicromousePlugin::OnMsg, this);
    }

    private: void OnMsg(ConstVector3dPtr &_msg)
    {
      //this->SetVelocity(_msg->x());
    }

    private: transport::NodePtr node;
    private: transport::SubscriberPtr sub;
    private: physics::ModelPtr model;
    private: physics::JointPtr joint;
    private: common::PID pid;
  };

  GZ_REGISTER_MODEL_PLUGIN(MicromousePlugin)
}

#endif
