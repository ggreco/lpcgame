#include "char.h"
#include "SDL_image.h"

Character::
Character(const std::string &xml) : AnimObj(xml) 
{
}

void Character::
do_step()
{
    if (step_) {
        MapSearchNode *next = step_;
        // se sono gia' a destinazione passo al prossimo step
        if (base_x() / MapSearchNode::RefMap->TileWidth() == step_->x &&
                base_y() / MapSearchNode::RefMap->TileHeight() == step_->y ) {
            std::cerr << "Reached: ";
            step_->PrintNodeInfo();
            next = astar_.GetSolutionNext();
        }

        if (next == NULL) {
            // arrived!
            if (delta_y_ > 0)
                Set("idle_down");
            else if (delta_y_ < 0)
                Set("idle_up");
            else if (delta_x_ > 0)
                Set("idle_right");
            else
                Set("idle_left");

            delta_x_ = 0;
            delta_y_ = 0;
            step_ = NULL;
            return;
        }

        if (next != step_) {
            std::cerr << "Moving to: ";
            next->PrintNodeInfo();
            // diagonal movements
            if (step_->x > next->x && step_->y > next->y) {
                delta_x_ = -3;
                delta_y_ = -3;
                Set("walk_up");               
            }
            else if  (step_->x < next->x && step_->y < next->y) {
                delta_x_ = 3;
                delta_y_ = 3;
                Set("walk_down");               
            }
            else if  (step_->x < next->x && step_->y > next->y) {
                delta_x_ = 3;
                delta_y_ = -3;
                Set("walk_up");               
            }
            else if  (step_->x > next->x && step_->y < next->y) {
                delta_x_ = -3;
                delta_y_ = 3;
                Set("walk_down");               
            }
            else if (step_->x > next->x) {
                delta_x_ = -3;
                delta_y_ = 0;
                Set("walk_left");               
            }
            else if (step_->x < next->x) {
                delta_x_ = 3;
                delta_y_ = 0;
                Set("walk_right");               
            }
            else if (step_->y > next->y) {
                delta_y_ = -3;
                delta_x_ = 0;
                Set("walk_up");               
            }
            else if (step_->y < next->y) {
                delta_y_ = 3;
                delta_x_ = 0;
                Set("walk_down");               
            }

            step_ = next;
        }

        x_ += delta_x_;
        y_ += delta_y_;
    }
}
