/*
 * bfs_initial_partitioner_test.cc
 *
 *  Created on: 29.04.2015
 *      Author: heuer
 */

#ifndef SRC_PARTITION_INITIAL_PARTITIONING_INITIAL_PARTITIONER_TEST_CC_
#define SRC_PARTITION_INITIAL_PARTITIONING_INITIAL_PARTITIONER_TEST_CC_

#include "gmock/gmock.h"
#include <vector>
#include <unordered_map>
#include <queue>

#include "lib/io/HypergraphIO.h"
#include "partition/Metrics.h"
#include "partition/initial_partitioning/InitialPartitionerBase.h"
#include "partition/initial_partitioning/BFSInitialPartitioner.h"
#include "partition/initial_partitioning/policies/StartNodeSelectionPolicy.h"
#include "partition/initial_partitioning/initial_partitioner_test_TestFixtures.h"

using ::testing::Eq;
using ::testing::Test;

using defs::Hypergraph;
using defs::HyperedgeIndexVector;
using defs::HyperedgeVector;
using defs::HyperedgeID;
using defs::PartitionID;



namespace partition {



TEST_F(ABFSInitialPartionerTest, BisectionBFSInitialPartionerTest) {

	partitioner->partition(2);
	std::vector<HypernodeID> partition_zero {0,1,2,3};
	std::vector<HypernodeID> partition_one {4,5,6};
	for(unsigned int i = 0; i < partition_zero.size(); i++) {
		ASSERT_EQ(hypergraph.partID(partition_zero[i]),0);
	}
	for(unsigned int i = 0; i < partition_one.size(); i++) {
		ASSERT_EQ(hypergraph.partID(partition_one[i]),1);
	}

	ASSERT_LE(hypergraph.partWeight(0),config.initial_partitioning.upper_allowed_partition_weight[0]);
	ASSERT_LE(hypergraph.partWeight(1),config.initial_partitioning.upper_allowed_partition_weight[1]);

}

TEST_F(ABFSInitialPartionerInstanceTest, KWayBFSInitialPartitionerTest) {

	partitioner->partition(config.initial_partitioning.k);

	ASSERT_LE(metrics::imbalance(*hypergraph),config.partition.epsilon);

	//Upper bounds of maximum partition weight should not be exceeded.
	HypernodeWeight heaviest_part = 0;
	for(PartitionID k = 0; k < config.initial_partitioning.k; k++) {
		ASSERT_LE(hypergraph->partWeight(k),config.initial_partitioning.upper_allowed_partition_weight[k]);
		if(heaviest_part < hypergraph->partWeight(k)) {
			heaviest_part = hypergraph->partWeight(k);
		}
	}

	//No partition weight should fall below under "lower_bound_factor" percent of the heaviest partition weight.
	double lower_bound_factor = 50.0;
	for(PartitionID k = 0; k < config.initial_partitioning.k; k++) {
		ASSERT_GE(hypergraph->partWeight(k),(lower_bound_factor/100.0)*heaviest_part);
	}

}

TEST_F(ABFSInitialPartionerTest, ABFSInitialPartionerPushesHypernodesIntoPriorityQueueTest) {

	std::queue<HypernodeID> q;
	std::unordered_map<HypernodeID,bool> in_queue;
	in_queue[0] = true;
	q.push(0);
	PartitionID tmp = -1;
	partitioner->pushIncidentHyperedgesIntoQueue(q,0,in_queue,tmp);
	for(HypernodeID hn = 0; hn < 5; hn++) {
		ASSERT_TRUE(in_queue[hn]);
	}
	for(HypernodeID hn = 5; hn < 7; hn++) {
		ASSERT_FALSE(in_queue[hn]);
	}
	std::vector<HypernodeID> expected_in_queue {0,2,1,3,4};
	for(unsigned int i = 0; i < expected_in_queue.size(); i++) {
		HypernodeID hn = q.front(); q.pop();
		ASSERT_EQ(hn,expected_in_queue[i]);
	}

	ASSERT_TRUE(q.empty());
}

TEST_F(ARandomInitialPartionerInstanceTest, BisectionRandomInitialPartitionerTest) {

	PartitionID k = 2;
	initializePartitioning(k);
	partitioner->partition(config.initial_partitioning.k);

	ASSERT_LE(metrics::imbalance(*hypergraph),config.partition.epsilon);

	//Upper bounds of maximum partition weight should not be exceeded.
	ASSERT_LE(hypergraph->partWeight(0),config.initial_partitioning.upper_allowed_partition_weight[0]);
	ASSERT_LE(hypergraph->partWeight(1),config.initial_partitioning.upper_allowed_partition_weight[1]);

}


TEST_F(ARandomInitialPartionerInstanceTest, KWayRandomInitialPartitionerTest) {

	PartitionID k = 32;
	initializePartitioning(k);
	partitioner->partition(config.initial_partitioning.k);

	ASSERT_LE(metrics::imbalance(*hypergraph),config.partition.epsilon);

	//Upper bounds of maximum partition weight should not be exceeded.
	HypernodeWeight heaviest_part = 0;
	for(PartitionID k = 0; k < config.initial_partitioning.k; k++) {
		ASSERT_LE(hypergraph->partWeight(k),config.initial_partitioning.upper_allowed_partition_weight[k]);
		if(heaviest_part < hypergraph->partWeight(k)) {
			heaviest_part = hypergraph->partWeight(k);
		}
	}

	//No partition weight should fall below under "lower_bound_factor" percent of the heaviest partition weight.
	double lower_bound_factor = 50.0;
	for(PartitionID k = 0; k < config.initial_partitioning.k; k++) {
		ASSERT_GE(hypergraph->partWeight(k),(lower_bound_factor/100.0)*heaviest_part);
	}

}



}


#endif /* SRC_PARTITION_INITIAL_PARTITIONING_INITIAL_PARTITIONER_TEST_CC_ */
