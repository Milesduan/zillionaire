#include "common.h"
#include "richman.h"
#include "display.h"

int main() {
    bool status;

    game_state = (GAME *)malloc(sizeof(GAME));

    while (1) {
        char return_back;
        printf("您是否要恢复之前游戏进度：y or n  ");
        return_back = getchar();
        if (return_back == 'y' || return_back == 'Y') {
            // TODO: recovery progress have saved!
            printf("recovery progress have saved!\n");
            goto Exit;
        }

        else if(return_back == 'n' || return_back == 'N') {
            // player choose to begin a new game
            break;
        }
    }

    status = ChooseRole();
    if(!status){
        printf("exit game ..\n");
        goto Exit;
    }

    InitMap();

    DispalyMap(game_state);

    GameStart();

    Exit:
    return 0;
}

bool ChooseRole()
{
    int player_num;
    int role_num;
    int exit_role[4] = {0,0,0,0};
    PLAYER *temp_player;
    bool ret = false, status;

    // choose player num
    while (1) {
        printf("请输入玩家数量2~4, 输入0退出: ");
        scanf("%d", &player_num);

        if (player_num >= 2 && player_num <= 4) {
            game_state->player_num = player_num;
            break;
        }

        else if(player_num == 0){
            goto Exit;
        }
    }

    // choose roles
    for(int i = 0; i < player_num; ){
        printf("选择%d号的角色: 1.钱夫人，2.阿土伯，3.金贝贝，4.孙小美:", i+1);
        scanf("%d", &role_num);

        if(role_num >=1 && role_num <= 4){
            ROLE_EXIST(role_num, exit_role, status);
            if(status){
                printf("该角色已被选择\n");
                continue;
            }
            exit_role[i] = role_num;
            temp_player = (PLAYER *)malloc(sizeof(PLAYER));
            INIT_PLAYER_HOUSE(temp_player);
            temp_player->player_id = i + 1;
            memcpy(temp_player->player_name, role_name[role_num-1], 16);
            temp_player->block_num = 0;
            temp_player->robot_num = 0;
            temp_player->bomb_num = 0;
            temp_player->cur_pos = 0;
            temp_player->money = INITIAL_MONEY;
            temp_player->point = 0;
            temp_player->sleep_time = 0;
            temp_player->next = game_state->player;
            game_state->player = temp_player;
        }

        else{
            printf("输入错误请重新输入.\n");
            continue;
        }

        ++ i;
    }

    game_state->current_player = game_state->player;
    ret = true;

Exit:
    return ret;
}

void InitMap()
{
    int i;
    for(i = 0; i < 69; ++i){
        game_state->map[i].ToolType = T_NO;
        game_state->map[i].house_level = 0;
        game_state->map[i].house_owner_id = 0;
        game_state->map[i].house_flag = SPACE;
        game_state->map[i].map_value = 0;
    }

    for(i = 1; i < 29; ++i) {
        game_state->map[i].map_value = 200;
    }

    for(; i < 35; ++i) {
        game_state->map[i].map_value = 500;
    }

    for (; i < 64; ++i) {
        game_state->map[i].map_value = 300;
    }

    for(; i < 70; ++i) {
        game_state->map[i].house_flag = MINERAL;
    }

    game_state->map[0].house_flag = START;
    game_state->map[14].house_flag = HOSPITAL;
    game_state->map[28].house_flag = TOOL;
    game_state->map[35].house_flag = GIFT;
    game_state->map[49].house_flag = PRISON;
    game_state->map[63].house_flag = MAGIC;

}

void GameStart() {
    char com_buf[10];
    int dele = 1;
    while (dele) {

        DispalyMap(game_state);

        // palyer play this round
        if (!game_state->current_player->sleep_time) {
            scanf("%s", com_buf);

            if(STR_EQU(COMMAND_ROLL, com_buf)){
                cmd_roll();
            }
            else if (STR_EQU(COMMAND_QUWRY, com_buf)){
                cmd_query();
            }
            else if (STR_EQU(COMMAND_HELP, com_buf)) {
                cmd_help();
            }
            else {
                printf("您输入的指令有误.\n");
                printf("\n please press any key to continue.");
                getchar();
                continue;
            }
        }

        // player skip this round
        else {
            -- game_state->current_player->sleep_time;
            CHECK_OUT_PLAYER(game_state);
        }
    }

}

void cmd_roll()
{
    short step;
    GET_STEP(step);

    // There is a bug: how to restore the flag when player leave.
    game_state->map[game_state->current_player->cur_pos].house_flag = get_house_flag(game_state->current_player->cur_pos);

    // player move
    game_state->current_player->cur_pos = (game_state->current_player->cur_pos + step) % 70;

    // update map
    game_state->map[game_state->current_player->cur_pos].house_flag = GET_PLAYER_FLAG;

    DispalyMap(game_state);
    printf("\n您获得的点数为：%d",step);
    printf("\n please press any key to continue.");
    getchar();
    CHECK_OUT_PLAYER(game_state);
}

void cmd_query()
{
    printf("\n亲爱%s：", game_state->current_player->player_name);
    printf("\n This is your query result");
    printf("\n please press any key to continue.");
    getchar();

}

void cmd_help()
{
    printf("\nthis is a help!");
    printf("\nTODO: show how to play.");
    printf("\n please press any key to continue.");
    getchar();
}




/**
*根据位置获取房屋符号
*/
char get_house_flag(int i) {
    char flag;
    if(i == 0) {
        flag = 'S';           // 起始点
    } else if((i >= 1 && i <= 13) || (i >= 15 && i <= 27)) {
        flag = '0';
    } else if(i == 14) {
        flag = 'H';
    } else if(i == 28) {
        flag = 'T';
    } else if(i >= 29 && i<= 34) {
        flag = '0';
    }else if(i == 35) {                 // 礼品屋
        flag = 'G';
    }else if((i >= 36 && i<= 48) || (i >= 50 && i <= 62)) {
        flag = '0';
    }else if(i==49) {
        flag = 'P';
    }else if(i == 63) {
        flag = 'M';
    }else {
        flag = '$';
    }
    return flag;
}


