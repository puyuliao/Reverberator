#include<bits/stdc++.h>
#include<stdint.h>
using namespace std;
#define IOS {cin.tie(0);ios_base::sync_with_stdio(false);}
#define N 1000000
#define printpair(x,y) (char)('A'+y) << (1+x)
#pragma GCC optimize("O3")
#define assert1 {}

// reverberator v1.3

/*references:
https://www.hanshq.net/othello.html#bitboards
http://www.cs.cmu.edu/~mmaxim/211-s03/bitboards.html
*/

typedef struct{
    uint64_t disks[2];
} othello_t;

typedef enum {
    DISK_BLACK = 0,
    DISK_WHITE = 1,
    DISK_EMPTY = 2
} disk_t;

typedef enum {
    PLAYER_BLACK = 0,
    PLAYER_WHITE = 1,
} player_t;

struct move_t{
    int score, boardidx;
    move_t(int s, int i) :score(s), boardidx(i){}
    bool operator < (const move_t &b) const{
        return score > b.score;
    }
};

const uint64_t zobrist[3][64] = {
    { 0xE39CDACBE4EAA637ULL, 0x9C4E42A196FABD0BULL, 0xBC7494100BA6BE4DULL, 0x2D3207184EA594B5ULL, 0x8E5E1C69BFBAC02EULL, 0x68793C822BC566BBULL, 0x095881769755860AULL, 0x933593CA266761BCULL, 0xF093B8D43B8F7608ULL, 0xD629ABE5DA83776AULL, 0x682C010162FD8D1FULL, 0x6A7FC0B0F9917933ULL, 0xD0803175138A85D4ULL, 0xDACCCBB6B60D9D5EULL, 0x3558C473D6E7EBFBULL, 0x88C362C3338DB1C8ULL, 0xF07A5F694E0D3371ULL, 0xC32CFA793947A351ULL, 0xFF427C1F5494A0F4ULL, 0x81704AB23CD8D7CFULL, 0xA1ECD373D862BBBBULL, 0xC68BAC38DC2D0109ULL, 0xC326BB4FEA3416D8ULL, 0x757565DAF3090FE8ULL, 0xAC7C32FC3D0A4695ULL, 0x0EF595B70D7718CAULL, 0x3A519940A87B9DAEULL, 0x55732B4EB4AE78B6ULL, 0x402CD11FB73934D6ULL, 0x1743C80D263421B6ULL, 0xFF1053FF1C05840FULL, 0x7E0DA1ED598312E0ULL, 0x7BE9A0D4D7501AEBULL, 0xD2C3B21FBE478A63ULL, 0x76C5CF8EBFC7FC07ULL, 0x1FC26461EB00631FULL, 0x1A1B10925A0BB4E8ULL, 0x29A7D51B8D42C743ULL, 0x47EF1088ACD8D802ULL, 0x3DB8EA2EAAF78B82ULL, 0x9483662D41DB56A3ULL, 0xA4C4C27616CF3E95ULL, 0xECA9075B925F59EDULL, 0xEEB538D78C8A0B98ULL, 0x6AD0F4224E63E8CCULL, 0x0EAD1EAD23951DEBULL, 0x99C0C0781E9D134EULL, 0xF72CB50DD1D8C67EULL, 0xA657B43DAE1F69BFULL, 0xF0BADDC8FD62F295ULL, 0x5DCDE02B2AE15A12ULL, 0xDF7A34EAC97EAFE9ULL, 0x8679D44CD32564D7ULL, 0xAF73641D95027C80ULL, 0x264BA2FD0E76A6DEULL, 0x486493BE0DEF5265ULL, 0x22415D8045F6F7E9ULL, 0xACBC110E15225330ULL, 0xA42A7D31C565807CULL, 0xA3BBF07934041370ULL, 0xC4B941E9BD216EDFULL, 0x1867C46986CF1795ULL, 0xC57CC0E64E4B6866ULL, 0x6138639CB28CB67CULL },
    { 0x964E71B1E36619A8ULL, 0x437C59248DB791D7ULL, 0x155A57BD30CEC67FULL, 0xE6708E3F8458B8D4ULL, 0xAA39915234979516ULL, 0x1ED1E0866B563F4BULL, 0x0D38EFDB0F6470DDULL, 0xD129DF2D175CFD54ULL, 0x1AD7272DF9375A55ULL, 0x777CD36F133B1575ULL, 0x297D03FA3D48BF5BULL, 0xB48919A4BD003504ULL, 0x1FDDF751F4E40D4FULL, 0x7D9B822EBB1CFC6CULL, 0x6BFFDEFFA6BAA80DULL, 0xAC85D17CDE127901ULL, 0x8C736924BE0B1603ULL, 0xB3EF2AAC74D13DF4ULL, 0x6D71717A92A4F411ULL, 0x9C3166EA82A81B3CULL, 0xA30F62DEA7F10F20ULL, 0x4C9B02409F754037ULL, 0xDE73AE5E7043416DULL, 0xD29B5017B877E0A7ULL, 0x16908312296271D2ULL, 0x060CDFCD62037EAAULL, 0x1E8C6A6CE94D067FULL, 0x84EEC68DFF9352E9ULL, 0x0D4A55875F3DA769ULL, 0xF680F517992548C7ULL, 0xDB529D36B49743C0ULL, 0x34E8903F36F9A64AULL, 0x903E0FF5A6B37961ULL, 0x45D75CDC6C8AFAE8ULL, 0x08FC6AB4B1ACF012ULL, 0xDB64ACF6086DF123ULL, 0x52993945E6F1E67CULL, 0x404A211F737C52E8ULL, 0xEF2F8B71720BD8D1ULL, 0x45DF610CC5DF6214ULL, 0x3D977DE80E22D290ULL, 0x11EB5925D9B594DAULL, 0x11FD59A16B19177EULL, 0xD274747A4076CFF3ULL, 0xAE73C2F2F4585077ULL, 0x4F0EF11E1626DABFULL, 0x12EBD82CB590F777ULL, 0x57609B904243878DULL, 0x1B1477181A97F4DFULL, 0x7B29E263E1FB1420ULL, 0x9C5F1AFA2E7847D0ULL, 0xF4FB2E7D5DD8DB60ULL, 0x59031C85AA110E30ULL, 0x48F71818223FF0A6ULL, 0x6856699578DA386CULL, 0x122AF4A9E152CA23ULL, 0xA5A41278E1852AFDULL, 0x9764FA36C11C0C3FULL, 0x9A63D07281C3CC15ULL, 0x3BE54F7AA236839FULL, 0x3D792555DAE23636ULL, 0xD41B28522F280F56ULL, 0x8BE5985BF34BEDEDULL, 0xEA6D149800CD1DF1ULL },
    { 0xB5A5EC928CDC091FULL, 0x155DEA7B60B84789ULL, 0x8299D37B4DEE2343ULL, 0x4C0309A408A65D15ULL, 0xD94CE5CCEEE3B7F3ULL, 0xACDD71B7AF872E2FULL, 0x9FDA53FEF0CAF34EULL, 0x9D8C5E16BDF63A69ULL, 0xEBDA64B0A1D1970DULL, 0x66618250C3126CB3ULL, 0x1C478F5BA78D793CULL, 0xD8D21AD3658A2788ULL, 0x499F887F0322C2C8ULL, 0x509FF3DD1D1AE318ULL, 0xF41FB227F35FA2A1ULL, 0xBE2899B2AACDDD0BULL, 0x609AF84187E46CF4ULL, 0xB75FE804ECAE0963ULL, 0x2A6F89CFFF72CEA4ULL, 0x667101187D58020AULL, 0x83A88430D909CA69ULL, 0x13B2470119264764ULL, 0x13ADDEB1E08F6793ULL, 0x658E8C6AA3BE7658ULL, 0xFBEFB43A174B3104ULL, 0xEC2AB2C19C1F71DAULL, 0xEED1F2D0C4E3264AULL, 0x6B25ED2ACBFFDA28ULL, 0xEF79545C398274BFULL, 0x07EEDA096DC16056ULL, 0x43917E95ECF0006CULL, 0xC746D8F416031E7CULL, 0x4317D0360AD5EDE8ULL, 0xEFDAE5BDBEFAB0A4ULL, 0xE3F8DEE382D6988DULL, 0x5E6DECA3FBE5D5DCULL, 0x3CA14A5ECA60D56EULL, 0x36547667BD48EB65ULL, 0x28BC892FFAE1CCA4ULL, 0xD2B5872305D20919ULL, 0x4CC98EB10152EE00ULL, 0xA26407CE227AF09EULL, 0x8317F285B69749D3ULL, 0x1D8EA4EDFF60493CULL, 0x245CAE3CAE5FD4B5ULL, 0x28B6169ACC4917ACULL, 0x3976AD0D52656520ULL, 0x910F3452B59527FFULL, 0x74EEB0D81749EE93ULL, 0x5945BA3AB8E50F62ULL, 0x4C90FD3372A5D029ULL, 0x5696A8BC5AD088CFULL, 0x85409B254D5CB0F0ULL, 0xC2A20F58F2205E74ULL, 0x2E017A307E08D28CULL, 0xCDBF46D1DAB51938ULL, 0xC384D35557C72F09ULL, 0x22CBDBEB463D1FFEULL, 0x9506F8C0D1089EEEULL, 0x11F76914F5DB7BF9ULL, 0xA8DFE21AE7A9665DULL, 0x70CCF69AEAB26126ULL, 0x4BB161A1DC1164BDULL, 0x1DCB1A91ADAA3FD4ULL }
};

uint64_t othello_hash(uint64_t my_disk, uint64_t opp_disk){
    uint64_t empty_disk = ~(my_disk | opp_disk);
    uint64_t r = 0, curr_disk;
    while (empty_disk){
        curr_disk = empty_disk & (~empty_disk + 1);
        r ^= zobrist[DISK_EMPTY][__lg(curr_disk)];
        empty_disk ^= curr_disk;
    }
    while (my_disk){
        curr_disk = my_disk & (~my_disk + 1);
        r ^= zobrist[DISK_BLACK][__lg(curr_disk)];
        my_disk ^= curr_disk;
    }
    while (opp_disk){
        curr_disk = opp_disk & (~opp_disk + 1);
        r ^= zobrist[DISK_WHITE][__lg(curr_disk)];
        opp_disk ^= curr_disk;
    }
    return r;
}

disk_t othello_disk(const othello_t *o, int r, int c){
    uint64_t mask = 1ULL << (r * 8 + c);

    assert1(r >= 0 && r <= 7);
    assert1(c >= 0 && c <= 7);

    if (o->disks[PLAYER_BLACK] & mask) {
        return DISK_BLACK;
    }
    if (o->disks[PLAYER_WHITE] & mask) {
        return DISK_WHITE;
    }
    return DISK_EMPTY;
}

void othello_set_disk(othello_t *o, int r, int c, disk_t s){
    uint64_t mask = 1ULL << (r * 8 + c);

    assert1(r >= 0 && r <= 7);
    assert1(c >= 0 && c <= 7);

    if (s == DISK_BLACK) o->disks[PLAYER_BLACK] |= mask;
    else if (s == DISK_WHITE) o->disks[PLAYER_WHITE] |= mask;
    else {
        o->disks[PLAYER_BLACK] &= ~mask;
        o->disks[PLAYER_WHITE] &= ~mask;
    }
}

void othello_init(othello_t *o){
    o->disks[PLAYER_BLACK] = 0;
    o->disks[PLAYER_WHITE] = 0;

    othello_set_disk(o, 3, 4, DISK_BLACK);
    othello_set_disk(o, 4, 3, DISK_BLACK);
    othello_set_disk(o, 3, 3, DISK_WHITE);
    othello_set_disk(o, 4, 4, DISK_WHITE);
}

static int popcount(uint64_t x){
    return __builtin_popcountll(x);
    // GCC only
    int r = 0;
    while (x) x &= x - 1, ++r;
    return r;
}

int othello_diskcount(const othello_t *o, player_t p){
    return popcount(o->disks[p]);
}

const int NUM_DIRS = 8;
static uint64_t shift(uint64_t disks, int dir){
    static const uint64_t MASKS[] = {
        0x7F7F7F7F7F7F7F7FULL, /* Right. */
        0x007F7F7F7F7F7F7FULL, /* Down-right. */
        0xFFFFFFFFFFFFFFFFULL, /* Down. */
        0x00FEFEFEFEFEFEFEULL, /* Down-left. */
        0xFEFEFEFEFEFEFEFEULL, /* Left. */
        0xFEFEFEFEFEFEFE00ULL, /* Up-left. */
        0xFFFFFFFFFFFFFFFFULL, /* Up. */
        0x7F7F7F7F7F7F7F00ULL  /* Up-right. */
    };
    static const uint64_t LSHIFTS[] = {
        0, /* Right. */
        0, /* Down-right. */
        0, /* Down. */
        0, /* Down-left. */
        1, /* Left. */
        9, /* Up-left. */
        8, /* Up. */
        7  /* Up-right. */
    };
    static const uint64_t RSHIFTS[] = {
        1, /* Right. */
        9, /* Down-right. */
        8, /* Down. */
        7, /* Down-left. */
        0, /* Left. */
        0, /* Up-left. */
        0, /* Up. */
        0  /* Up-right. */
    };
    if (dir < (NUM_DIRS >> 1)) return (disks >> RSHIFTS[dir]) & MASKS[dir];
    return (disks << LSHIFTS[dir]) & MASKS[dir];
}

static uint64_t generate_moves(uint64_t my_disks, uint64_t opp_disks){
    uint64_t x;
    uint64_t empty_disks = ~(my_disks | opp_disks);
    uint64_t legal_moves = 0;

    assert1((my_disks & opp_disks) == 0 && "Disk sets should be disjoint.");

    for (int dir = 0; dir < NUM_DIRS; dir++) {
        // Get opponent disks adjacent to my disks in direction dir. 
        x = shift(my_disks, dir) & opp_disks;
        // Add opponent disks adjacent to those, and so on.
        x |= shift(x, dir) & opp_disks;
        x |= shift(x, dir) & opp_disks;
        x |= shift(x, dir) & opp_disks;
        x |= shift(x, dir) & opp_disks;
        x |= shift(x, dir) & opp_disks;

        // Empty cells adjacent to those are valid moves.
        legal_moves |= shift(x, dir) & empty_disks;
    }
    return legal_moves;
}

bool othello_has_move(const othello_t *o, player_t p){
    return generate_moves(o->disks[p], o->disks[p ^ 1]) != 0;
}

bool othello_is_move(const othello_t *o, player_t p, int r, int c){
    uint64_t mask = 1ULL << (r * 8 + c);

    assert1(r >= 0 && r <= 7);
    assert1(c >= 0 && c <= 7);

    return (generate_moves(o->disks[p], o->disks[p ^ 1]) & mask) != 0;
}

static void resolve_move(uint64_t *my_disks, uint64_t *opp_disks, int boardidx){
    //	othello_t db; db.disks[0] = *my_disks; db.disks[1] = *opp_disks;
    //	cout << "current : " << printpair(7-boardidx/8,7-boardidx%8) << '\n';
    //	showboard(&db);  
    //	cout << "\n";
    uint64_t x, bounding_disk;
    uint64_t captured_disks = 0;
    uint64_t new_disk = 1ULL << boardidx;

    assert1(boardidx < 64 && "Move must be within the board.");
    assert1((*my_disks & *opp_disks) == 0 && "Disk sets must be disjoint.");
    assert1(!((*my_disks | *opp_disks) & new_disk) && "Target not empty!");

    *my_disks |= new_disk;

    for (int dir = 0; dir < NUM_DIRS; dir++) {
        x = shift(new_disk, dir) & *opp_disks;

        x |= shift(x, dir) & *opp_disks;
        x |= shift(x, dir) & *opp_disks;
        x |= shift(x, dir) & *opp_disks;
        x |= shift(x, dir) & *opp_disks;
        x |= shift(x, dir) & *opp_disks;

        bounding_disk = shift(x, dir) & *my_disks;
        captured_disks |= (bounding_disk ? x : 0);
    }

    *my_disks ^= captured_disks;
    *opp_disks ^= captured_disks;

    assert1(!(*my_disks & *opp_disks) && "The sets must still be disjoint.");
}

void othello_make_move(othello_t *o, player_t p, int r, int c){
    resolve_move(&o->disks[p], &o->disks[p ^ 1], r * 8 + c);
}

static void frontier_disks(uint64_t my_disks, uint64_t opp_disks, uint64_t *my_frontier, uint64_t *opp_frontier){
    uint64_t empty_cells = ~(my_disks | opp_disks);
    uint64_t x;

    *my_frontier = *opp_frontier = 0;

    for (int dir = 0; dir < NUM_DIRS; dir++) {
        x = shift(empty_cells, dir);
        *my_frontier |= x & my_disks;
        *opp_frontier |= x & opp_disks;
    }
}

static void get_edge_stable(uint64_t my_disks,uint64_t *my_stable_disks){
	static const uint64_t CORNER_MASK = 0x8100000000000081ULL;
	uint64_t lr,ud;
	lr = my_disks & CORNER_MASK;
	lr |= shift(lr,2) & my_disks;
	lr |= shift(lr,2) & my_disks;
	lr |= shift(lr,2) & my_disks;
	lr |= shift(lr,2) & my_disks;
	lr |= shift(lr,2) & my_disks;
	lr |= shift(lr,2) & my_disks;
	
	lr |= shift(lr,6) & my_disks;
	lr |= shift(lr,6) & my_disks;
	lr |= shift(lr,6) & my_disks;
	lr |= shift(lr,6) & my_disks;
	lr |= shift(lr,6) & my_disks;
	lr |= shift(lr,6) & my_disks;
	
	ud = my_disks & CORNER_MASK;
	ud |= shift(ud,0) & my_disks;
	ud |= shift(ud,0) & my_disks;
	ud |= shift(ud,0) & my_disks;
	ud |= shift(ud,0) & my_disks;
	ud |= shift(ud,0) & my_disks;
	ud |= shift(ud,0) & my_disks;
	
	ud |= shift(ud,4) & my_disks;
	ud |= shift(ud,4) & my_disks;
	ud |= shift(ud,4) & my_disks;
	ud |= shift(ud,4) & my_disks;
	ud |= shift(ud,4) & my_disks;
	ud |= shift(ud,4) & my_disks;
	
	*my_stable_disks = lr | ud;
} 

const int WIN_BONUS = 1 << 20;
static int eval(uint64_t my_disks, uint64_t opp_disks, uint64_t my_moves, uint64_t opp_moves){
    static const uint64_t CORNER_MASK = 0x8100000000000081ULL;
    static const uint64_t X_MASK_UL = 0x0040000000000000ULL;
    static const uint64_t X_MASK_UR = 0x0002000000000000ULL;
    static const uint64_t X_MASK_DL = 0x0000000000004000ULL;
    static const uint64_t X_MASK_DR = 0x0000000000000200ULL;

    int my_disk_count, opp_disk_count;
    uint64_t my_corners, opp_corners;
    uint64_t my_frontier, opp_frontier;
    uint64_t my_stable, opp_stable;
    uint64_t my_x = 0,opp_x = 0,corner_occupied;
    int score = 0;

    if (!my_moves && !opp_moves) {
        my_disk_count = popcount(my_disks);
        opp_disk_count = popcount(opp_disks);
        if (my_disk_count > opp_disk_count) return (64 - opp_disk_count * 2) * WIN_BONUS;
        return (my_disk_count * 2 - 64) * WIN_BONUS;
    }

    my_corners = my_disks & CORNER_MASK;
    opp_corners = opp_disks & CORNER_MASK;
	
	corner_occupied = my_corners | opp_corners;
	if(!(corner_occupied & 0x8000000000000000ULL)){
		my_x |= my_disks & X_MASK_UL;
		opp_x  |= opp_disks & X_MASK_UL;
	}
	if(!(corner_occupied & 0x0100000000000000ULL)){
		my_x |= my_disks & X_MASK_UR;
		opp_x  |= opp_disks & X_MASK_UR;
	}
	if(!(corner_occupied & 0x0000000000000080ULL)){
		my_x |= my_disks & X_MASK_DL;
		opp_x  |= opp_disks & X_MASK_DL;
	}
	if(!(corner_occupied & 0x0000000000000001ULL)){
		my_x |= my_disks & X_MASK_DR;
		opp_x  |= opp_disks & X_MASK_DR;
	}

    frontier_disks(my_disks, opp_disks, &my_frontier, &opp_frontier);
    get_edge_stable(my_disks, &my_stable);
    get_edge_stable(opp_disks, &opp_stable);

    score += (popcount(my_corners) - popcount(opp_corners)) * 1000;
    score += (popcount(my_moves) - popcount(opp_moves)) * 80;
    score += (popcount(my_frontier) - popcount(opp_frontier)) * -100;
    score += (popcount(my_stable) - popcount(opp_stable)) * 30;

    return score;
}

static int endgamesolver(uint64_t my_disks, uint64_t opp_disks, int max_depth, int alpha, int beta, int *best_move){
    uint64_t my_moves, opp_moves;
    uint64_t my_new_disks, opp_new_disks;

    my_moves = generate_moves(my_disks, opp_disks);
    opp_moves = generate_moves(opp_disks, my_disks);

    if (!my_moves && opp_moves) return -endgamesolver(opp_disks, my_disks, max_depth, -beta, -alpha, best_move);
    if (!max_depth || (!my_moves && !opp_moves)) return eval(my_disks, opp_disks, my_moves, opp_moves);

    assert1(alpha < beta);

    int best = -INT_MAX, s;
    for (int i = 0; i<64; i++) if (my_moves & (1ULL << i)){
        my_new_disks = my_disks;
        opp_new_disks = opp_disks;
        resolve_move(&my_new_disks, &opp_new_disks, i);

        s = -endgamesolver(opp_new_disks, my_new_disks, max_depth - 1, -beta, -alpha, NULL);
        if (s > best){
            best = s;
            if (best_move) *best_move = i;
            alpha = s > alpha ? s : alpha;
            if (alpha >= beta) break;
        }
    }
    return best;
}

const double timelim = 0.92;
const int timecut = INT_MIN;
int nowclocks;
unordered_map<uint64_t, vector<move_t> > mp;

static int negamax(uint64_t my_disks, uint64_t opp_disks, int max_depth, int alpha, int beta, int *best_move){
    if ((clock() - nowclocks)*1.0 / CLOCKS_PER_SEC >= timelim) return timecut;
    uint64_t my_moves, opp_moves;
    uint64_t my_new_disks, opp_new_disks;

    my_moves = generate_moves(my_disks, opp_disks);
    opp_moves = generate_moves(opp_disks, my_disks);

    if (!my_moves && opp_moves) return -negamax(opp_disks, my_disks, max_depth, -beta, -alpha, best_move);
    if (!max_depth || (!my_moves && !opp_moves)) return eval(my_disks, opp_disks, my_moves, opp_moves);

    uint64_t hhval = othello_hash(my_disks, opp_disks);
    if (mp.find(hhval) == mp.end()) {
        vector<move_t> v;
        for (int i = 0; i<64; i++) if (my_moves & (1ULL << i))
            v.emplace_back(move_t(0, i));
        mp[hhval].swap(v);
    }
    assert1(alpha < beta);

    int best = -INT_MAX;
    for (move_t &i : mp[hhval]) {
        my_new_disks = my_disks;
        opp_new_disks = opp_disks;
        resolve_move(&my_new_disks, &opp_new_disks, i.boardidx);

        i.score = -negamax(opp_new_disks, my_new_disks, max_depth - 1, -beta, -alpha, NULL);
        if (i.score == timecut) return timecut;
        if (i.score > best){
            best = i.score;
            if (best_move) *best_move = i.boardidx;
            alpha = i.score > alpha ? i.score : alpha;
            if (alpha >= beta) break;
        }
    }
    sort(mp[hhval].begin(), mp[hhval].end());
    return best;
}

int othello_compute_move(const othello_t *o, player_t p, int *r, int *c){
    int move_idx, val = 0;
    mp.clear();

    if (popcount(~(o->disks[p] | o->disks[p ^ 1])) <= 13)
        val = endgamesolver(o->disks[p], o->disks[p ^ 1], 13, -INT_MAX, INT_MAX, &move_idx);
    else {
        nowclocks = clock();
        int dep, curr_idx, curr_val;
        for (dep = 0; dep <= 64; dep++){
            curr_val = negamax(o->disks[p], o->disks[p ^ 1], dep, -INT_MAX, INT_MAX, &curr_idx);
            if ((clock() - nowclocks)*1.0 / CLOCKS_PER_SEC < timelim)
                move_idx = curr_idx, val = curr_val;
            else break;
        }
    }
    *r = move_idx / 8;
    *c = move_idx % 8;
    return val;
}

void showboard(const othello_t *o){
    cout << " ";
    for (int i = 0; i<8; i++) cout << i + 1; cout << '\n';
    for (int i = 0; i<8; i++){
        cout << (char)('A' + i);
        for (int j = 0; j<8; j++){
            int idx = i * 8 + j;
            if (o->disks[PLAYER_BLACK] & (1ULL << idx)) cout << "O";
            else if (o->disks[PLAYER_WHITE] & (1ULL << idx)) cout << "X";
            else cout << ".";
        }
        cout << '\n';
    }
}

int main()
{
    //IOS;
    int pass = 0;
    int user;
    cout << "Choose playing color: \n0 - Black. \n1 - White.\n>>> ";
    cin >> user;
    othello_t G;
    othello_init(&G);
    player_t nowplayer = PLAYER_BLACK;
    while (othello_has_move(&G, PLAYER_BLACK) || othello_has_move(&G, PLAYER_WHITE)){
        showboard(&G);
        if (othello_has_move(&G, nowplayer)){
            if (nowplayer == user){
                string s;
                cout << ">>> ";
                cin >> s;
                int a = toupper(s[0]) - 'A';
                int b = s[1] - '1';
                if (0 <= a && a<8 && 0 <= b && b < 8 && othello_is_move(&G, nowplayer, a, b))
                    resolve_move(&G.disks[nowplayer], &G.disks[nowplayer ^ 1], a * 8 + b);
                else continue;
                pass = 0;
            }
            else{
                if (pass++ && (user == PLAYER_BLACK || user == PLAYER_WHITE))
                    this_thread::sleep_for(chrono::milliseconds(500));
                int r, c, val;
                val = othello_compute_move(&G, nowplayer, &r, &c);
                cout << "compute move : " << printpair(c, r) << '\n';
                cout << "val = " << val << '\n';
                resolve_move(&G.disks[nowplayer], &G.disks[nowplayer ^ 1], r * 8 + c);
            }
        }
        nowplayer = (player_t)(nowplayer ^ 1);
    }
    showboard(&G);
    return 0;
}
