#include <stdio.h>
#include <string.h>

#define MAX_CANDIDATE 20
#define MAX_VOTE 999

int check_winner(int vote_count[], int total_votes, int active_candidates[], int total_candidate) {
    double half_votes = total_votes / 2.0;
    
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i] && vote_count[i] >= half_votes) {
            return i; 
        }
    }
    return -1; 
}

int find_candidate_to_eliminate(int vote_count[], int active_candidates[], int total_candidate) {
    int min_votes = MAX_VOTE + 1;
    int candidate_to_eliminate = -1;
    
    for (int i = 0; i < total_candidate; i++) {
        if (active_candidates[i] && vote_count[i] < min_votes) {
            min_votes = vote_count[i];
            candidate_to_eliminate = i;
        }
    }
    
    return candidate_to_eliminate;
}

void redistribute(int votes[][MAX_CANDIDATE], int vote_count[], int active_candidates[], 
                 int eliminated_candidate, int total_votes, int total_candidate) {
    
    for (int ballot = 0; ballot < total_votes; ballot++) {
        
        // Tìm lựa chọn hiện tại của phiếu (trong số người còn lại)
        int current_choice = -1;
        int best_preference = total_candidate + 1;
        
        for (int candidate = 0; candidate < total_candidate; candidate++) {
            if (active_candidates[candidate] && votes[ballot][candidate] < best_preference) {
                best_preference = votes[ballot][candidate];
                current_choice = candidate;
            }
        }
        
        // Nếu lựa chọn hiện tại là người bị loại
        if (current_choice == eliminated_candidate) {
            vote_count[eliminated_candidate]--;
            
            // Loại người này khỏi active (tạm thời)
            active_candidates[eliminated_candidate] = 0;
            
            // Tìm lựa chọn mới
            int new_choice = -1;
            int new_best = total_candidate + 1;
            
            for (int candidate = 0; candidate < total_candidate; candidate++) {
                if (active_candidates[candidate] && 
                    votes[ballot][candidate] < new_best) {
                    new_best = votes[ballot][candidate];
                    new_choice = candidate;
                }
            }
            
            if (new_choice != -1) {
                vote_count[new_choice]++;
            }
            
            // Khôi phục active (để xử lý phiếu tiếp theo)
            active_candidates[eliminated_candidate] = 0; // Giữ nguyên = 0
        }
    }
}

int stage2(int votes[][MAX_CANDIDATE], char candidate[][20], int total_candidate, int total_votes) {
    int vote_count[MAX_CANDIDATE] = {0};
    int active_candidates[MAX_CANDIDATE];
    
    // Khởi tạo các ứng viên còn hoạt động
    for (int i = 0; i < total_candidate; i++) {
        active_candidates[i] = 1;
    }
    
    for (int v = 0; v < total_votes; v++) {
        for (int c = 0; c < total_candidate; c++) {
            if (votes[v][c] == 1) {
                vote_count[c]++;
                break;
            }
        }
    }
    
    int round = 1;
    while (1) {
        printf("round %d:\n", round);
        
        for (int i = 0; i < total_candidate; i++) {
            if (active_candidates[i]) {
                double vote_ratio = 100.0*vote_count[i]/total_candidate;
                printf("    %-20s:   %d votes, %5.1lf%%\n", candidate[i], vote_count[i], vote_ratio);
            }
        }
        
        // Kiểm tra người thắng cử
        int winner = check_winner(vote_count, total_votes, active_candidates, total_candidate);
        if (winner != -1) {
            printf("%s is declared elected\n", candidate[winner]);
            return winner;
        }
        
        // Tìm ứng viên có số phiếu thấp nhất để loại bỏ
        int eliminated = find_candidate_to_eliminate(vote_count, active_candidates, total_candidate);
        
        printf("%s is eliminated\n", candidate[eliminated]);
        
        // Phân phối lại phiếu
        redistribute(votes, vote_count, active_candidates, eliminated, total_votes, total_candidate);

        // Loại bỏ ứng viên
        active_candidates[eliminated] = 0;
        
        round++;
    }
}

// Hàm main để test
int main(int argc, char *argv[]) {
    int total_candidate = 5;
    char candidate[MAX_CANDIDATE][20] = {"Ann", "Bob", "Carl", "Dave", "Eli"};
    int votes[MAX_VOTE][MAX_CANDIDATE] = {
        {1, 2, 4, 3, 5},  
        {1, 4, 2, 3, 5},  
        {2, 5, 4, 3, 1},  
        {2, 1, 3, 4, 5}   
    };
    int total_votes = 4;
    
    int winner = stage2(votes, candidate, total_candidate, total_votes);
    
    return 0;
}
