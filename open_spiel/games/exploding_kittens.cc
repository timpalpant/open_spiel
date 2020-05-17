// Copyright 2019 DeepMind Technologies Ltd. All rights reserved.
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

#include "open_spiel/games/exploding_kittens.h"

#include <algorithm>
#include <map>
#include <utility>

#include "open_spiel/abseil-cpp/absl/algorithm/container.h"
#include "open_spiel/abseil-cpp/absl/strings/str_format.h"
#include "open_spiel/abseil-cpp/absl/strings/str_join.h"
#include "open_spiel/game_parameters.h"
#include "open_spiel/games/exploding_kittens/exploding_kittens_utils.h"
#include "open_spiel/spiel_utils.h"

namespace open_spiel {
namespace exploding_kittens {
namespace {

const GameType kGameType{
    /*short_name=*/"exploding_kittens",
    /*long_name=*/"Expoding Kittens",
    GameType::Dynamics::kSequential,
    GameType::ChanceMode::kExplicitStochastic,
    GameType::Information::kImperfectInformation,
    GameType::Utility::kZeroSum,
    GameType::RewardModel::kTerminal,
    /*max_num_players=*/2,
    /*min_num_players=*/2,
    /*provides_information_state_string=*/false,
    /*provides_information_state_tensor=*/false,
    /*provides_observation_string=*/true,
    /*provides_observation_tensor=*/true,
    /*parameter_specification=*/
    {
        {"deck", GameParameter(0)},
    }};

std::shared_ptr<const Game> Factory(const GameParameters& params) {
  return std::shared_ptr<const Game>(new ExplodingKittensGame(params));
}

REGISTER_SPIEL_GAME(kGameType, Factory);

}  // namespace

ExplodingKittensState::ExplodingKittensState(std::shared_ptr<const Game> game, int deck)
    : State(std::move(game)),
      deck_(deck) {}

int ExplodingKittensState::CurrentPlayer() const {
  if (IsTerminal()) {
    return kTerminalPlayerId;
  } else {
    return cur_player_;
  }
}

void ExplodingKittensState::DoApplyAction(Action action) {
  switch (phase_) {
    case Phase::kDeal:
      return ApplyDealAction(action);
    case Phase::kPlayTurn:
      return ApplyPlayTurnAction(action);
    case Phase::kGiveCard:
      return ApplyGiveCardAction(action);
    case Phase::kShuffleDrawPile:
      return ApplyShuffleDrawPileAction(action);
    case Phase::kMustDefuse:
      return ApplyMustDefuseAction(action);
    case Phase::kInsertKittenRandom:
      return ApplyInsertKittenRandomAction(action);
    case Phase::kGameOver:
      SpielFatalError("Cannot act in terminal states");
  }
}

std::vector<Action> ExplodingKittensState::LegalActions() const {
  return {};
}

std::vector<std::pair<Action, double>> ExplodingKittensState::ChanceOutcomes() const {
  SPIEL_CHECK_TRUE(IsChanceNode());
  std::vector<std::pair<Action, double>> outcomes;
  outcomes.reserve(stock_size_);
  const double p = 1.0 / stock_size_;
  for (int card = 0; card < kNumCards; ++card) {
    // This card is still in the deck, prob is 1/stock_size_.
    if (deck_[card]) outcomes.push_back({card, p});
  }
  return outcomes;
}

std::string ExplodingKittensState::ToString() const {
  std::string rv;
  return rv;
}

std::vector<double> ExplodingKittensState::Returns() const {
  if (!IsTerminal()) {
    return std::vector<double>(kNumPlayers, 0.0);
  }
  std::vector<double> returns(kNumPlayers, 0.0);
  // If neither player knocked both players get 0.
  return returns;
}

std::unique_ptr<State> ExplodingKittensState::Clone() const {
  return std::unique_ptr<State>(new ExplodingKittensState(*this));
}

std::string ExplodingKittensState::ObservationString(Player player) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, num_players_);

  // Built from ObservationTensor to provide an extra check.
  std::vector<double> tensor(game_->ObservationTensorSize());
  ObservationTensor(player, &tensor);
}

void ExplodingKittensState::ObservationTensor(Player player,
                                      std::vector<double>* values) const {
  SPIEL_CHECK_GE(player, 0);
  SPIEL_CHECK_LT(player, kNumPlayers);

}

ExplodingKittensGame::ExplodingKittensGame(const GameParameters& params)
    : Game(kGameType, params),
      deck_(ParameterValue<int>("deck")) {
  SPIEL_CHECK_GE(deck_, 0);
  SPIEL_CHECK_LT(deck_, kNumDecks);
}

}  // namespace exploding_kittens
}  // namespace open_spiel
