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

#ifndef OPEN_SPIEL_GAMES_EXPLODING_KITTENS_H_
#define OPEN_SPIEL_GAMES_EXPLODING_KITTENS_H_

// Implementation of the Exploding Kittens card game:
// https://en.wikipedia.org/wiki/Exploding_Kittens
//
// This implementation matches the 2-player Core Deck on the iOS app.
//
// Parameters:
//  "deck"            int    which deck to use             (default = 0)

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "open_spiel/spiel.h"

namespace open_spiel {
namespace exploding_kittens {

inline constexpr int kNumDecks = 1;
inline constexpr int kNumPlayers = 2;
inline constexpr int kNumDistinctCardTypes = 9;
inline constexpr int kNumSkipCards = 5;
inline constexpr int kNumSlap1xCards = 3;
inline constexpr int kNumSlap2xCards = 1;
inline constexpr int kNumSeeTheFutureCards = 3;
inline constexpr int kNumShuffleCards = 2;
inline constexpr int kNumDrawFromTheBottomCards = 2;
inline constexpr int kNumCatCards = 3;
inline constexpr int kNumCardsInDeck = 
    kNumSkipCards + kNumSlap1xCards + kNumSlap1xCards
    + kNumSeeTheFutureCards + kNumShuffleCards
    + kNumDrawFromTheBottomCards + kNumCatCards
    + kNumPlayers // Defuse cards
    + 1;          // Exploding kitten
inline constexpr int kHandSize = 5;
// Size of draw pile when play begins.
inline constexpr int kMaxStockSize = kNumCardsInDeck - kNumPlayers*kHandSize;
inline constexpr int kNumDistinctActions = 5;
inline constexpr int kObservationTensorSize = 10;

class ExplodingKittensState : public State {
 public:
  explicit ExplodingKittensState(std::shared_ptr<const Game> game, int deck);
  Player CurrentPlayer() const override;
  std::string ActionToString(Player player, Action action) const override;
  std::string ToString() const override;
  bool IsTerminal() const override { return phase_ == Phase::kGameOver; }
  std::vector<double> Returns() const override;
  std::string ObservationString(Player player) const override;
  void ObservationTensor(Player player,
                         std::vector<double>* values) const override;
  std::unique_ptr<State> Clone() const override;
  std::vector<Action> LegalActions() const override;
  std::vector<std::pair<Action, double>> ChanceOutcomes() const override;

 protected:
  void DoApplyAction(Action action) override;

 private:
  enum class Phase {
    kDeal,
    kPlayTurn,
    kGiveCard,
    kShuffleDrawPile,
    kMustDefuse,
    kInsertKittenRandom,
    kGameOver
  };

  inline static constexpr std::array<absl::string_view, 7> kPhaseString = {
      "Deal", "PlayTurn", "GiveCard", "ShuffleDrawPile",
      "MustDefuse", "InsertKittenRandom", "GameOver"};

  int Opponent(int player) const { return 1 - player; }

  Phase phase_ = Phase::kDeal;
  Player cur_player_ = kChancePlayerId;
  Player prev_player_ = kChancePlayerId;

};

class ExplodingKittensGame : public Game {
 public:
  explicit ExplodingKittensGame(const GameParameters& params);

  int NumDistinctActions() const override { return kNumDistinctActions; }
  int MaxChanceOutcomes() const override { return kNumCards; }
  int NumPlayers() const override { return kNumPlayers; }
  double MinUtility() const override { return -1.0; }
  double MaxUtility() const override { return 1.0; }
  double UtilitySum() const override { return 0; }
  std::unique_ptr<State> NewInitialState() const override {
    return std::unique_ptr<State>(
        new ExplodingKittensState(shared_from_this(), 0))
  }
  std::shared_ptr<const Game> Clone() const override {
    return std::shared_ptr<const Game>(new ExplodingKittensGame(*this));
  }
  std::vector<int> ObservationTensorShape() const override {
    return {kObservationTensorSize};
  }
  // All games should terminate before reaching this upper bound.
  int MaxGameLength() const override { return 100; }

 private:
  const int deck_;
};

}  // namespace exploding_kittens
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_EXPLODING_KITTENS_H_
