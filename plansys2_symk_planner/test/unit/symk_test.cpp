// Copyright 2024 KAS Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

#include "ament_index_cpp/get_package_share_directory.hpp"

#include "gtest/gtest.h"
#include "plansys2_symk_planner/symk_planner.hpp"

#include "pluginlib/class_loader.hpp"
#include "pluginlib/class_list_macros.hpp"
#include "plansys2_core/PlanSolverBase.hpp"

void test_plan_generation(const std::string & argument = "")
{
  std::string pkgpath = ament_index_cpp::get_package_share_directory("plansys2_symk_planner");
  std::ifstream domain_ifs(pkgpath + "/pddl/domain_simple.pddl");
  std::string domain_str((
      std::istreambuf_iterator<char>(domain_ifs)),
    std::istreambuf_iterator<char>());

  std::ifstream problem_ifs(pkgpath + "/pddl/problem_simple_1.pddl");
  std::string problem_str((
      std::istreambuf_iterator<char>(problem_ifs)),
    std::istreambuf_iterator<char>());

  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_node");
  auto planner = std::make_shared<plansys2::SymkPlanner>();
  planner->configure(node, "Symk");
  node->set_parameter(rclcpp::Parameter("Symk.arguments", argument));

  auto plan = planner->getPlan(domain_str, problem_str, "generate_plan_good");

  ASSERT_TRUE(plan);
  ASSERT_EQ(plan.value().items.size(), 3);
  ASSERT_EQ(plan.value().items[0].action, "(move leia kitchen bedroom)");
  ASSERT_EQ(plan.value().items[1].action, "(approach leia bedroom jack)");
  ASSERT_EQ(plan.value().items[2].action, "(talk leia jack jack m1)");
}

TEST(symk_planner, generate_plan_good)
{
  test_plan_generation();
}

// TEST(symk_planner, generate_plan_good_with_argument)
// {
//   test_plan_generation("-e");
// }

TEST(symk_planner, load_symk_plugin)
{
  try {
    pluginlib::ClassLoader<plansys2::PlanSolverBase> lp_loader(
      "plansys2_core", "plansys2::PlanSolverBase");
    plansys2::PlanSolverBase::Ptr plugin =
      lp_loader.createUniqueInstance("plansys2/SymkPlanner");
    ASSERT_TRUE(true);
  } catch (std::exception & e) {
    std::cerr << e.what() << std::endl;
    ASSERT_TRUE(false);
  }
}

TEST(symk_planner, check_1_ok_domain)
{
  std::string pkgpath = ament_index_cpp::get_package_share_directory("plansys2_symk_planner");
  std::ifstream domain_ifs(pkgpath + "/pddl/domain_1_ok.pddl");
  std::string domain_str((
      std::istreambuf_iterator<char>(domain_ifs)),
    std::istreambuf_iterator<char>());

  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_node");
  auto planner = std::make_shared<plansys2::SymkPlanner>();
  planner->configure(node, "Symk");

  bool result = planner->is_valid_domain(domain_str, "check_1_ok_domain");

  ASSERT_TRUE(result);
}


TEST(symk_planner, check_2_error_domain)
{
  std::string pkgpath = ament_index_cpp::get_package_share_directory("plansys2_symk_planner");
  std::ifstream domain_ifs(pkgpath + "/pddl/domain_2_error.pddl");
  std::string domain_str((
      std::istreambuf_iterator<char>(domain_ifs)),
    std::istreambuf_iterator<char>());

  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_node");
  auto planner = std::make_shared<plansys2::SymkPlanner>();
  planner->configure(node, "Symk");

  bool result = planner->is_valid_domain(domain_str, "check_2_error_domain");

  ASSERT_FALSE(result);
}


TEST(symk_planner, generate_plan_unsolvable)
{
  std::string pkgpath = ament_index_cpp::get_package_share_directory("plansys2_symk_planner");
  std::ifstream domain_ifs(pkgpath + "/pddl/domain_simple.pddl");
  std::string domain_str((
      std::istreambuf_iterator<char>(domain_ifs)),
    std::istreambuf_iterator<char>());

  std::ifstream problem_ifs(pkgpath + "/pddl/problem_simple_2.pddl");
  std::string problem_str((
      std::istreambuf_iterator<char>(problem_ifs)),
    std::istreambuf_iterator<char>());

  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_node");
  auto planner = std::make_shared<plansys2::SymkPlanner>();
  planner->configure(node, "Symk");

  auto plan = planner->getPlan(domain_str, problem_str);

  ASSERT_FALSE(plan);
}

TEST(symk_planner, generate_plan_error)
{
  std::string pkgpath = ament_index_cpp::get_package_share_directory("plansys2_symk_planner");
  std::ifstream domain_ifs(pkgpath + "/pddl/domain_simple.pddl");
  std::string domain_str((
      std::istreambuf_iterator<char>(domain_ifs)),
    std::istreambuf_iterator<char>());

  std::ifstream problem_ifs(pkgpath + "/pddl/problem_simple_3.pddl");
  std::string problem_str((
      std::istreambuf_iterator<char>(problem_ifs)),
    std::istreambuf_iterator<char>());

  auto node = rclcpp_lifecycle::LifecycleNode::make_shared("test_node");
  auto planner = std::make_shared<plansys2::SymkPlanner>();
  planner->configure(node, "Symk");

  auto plan = planner->getPlan(domain_str, problem_str);

  ASSERT_FALSE(plan);
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  rclcpp::init(argc, argv);

  return RUN_ALL_TESTS();
}
