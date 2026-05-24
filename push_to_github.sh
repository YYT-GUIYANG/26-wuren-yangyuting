#!/bin/bash
#推送到: https://github.com/YYT-GUIYANG/26-wuren-yangyuting.git
#用法: cd 作业2目录后 bash push_to_github.sh
set -e
cd "$(dirname "$0")"
REMOTE="https://github.com/YYT-GUIYANG/26-wuren-yangyuting.git"
echo "=== 作业2 上传到 GitHub ==="
echo "仓库: $REMOTE"
if [ ! -d .git ]; then
    git init -b main
fi
git remote remove origin 2>/dev/null || true
git remote add origin "$REMOTE"
git add README.md .gitignore \
    linear_regression.cpp ridge_regression.cpp prepare_data.py \
    01_多元线性回归.ipynb 02_正则化回归.ipynb \
    data/housing.csv data/train.csv data/test.csv \
    upload_github.sh push_to_github.sh 2>/dev/null || true
git add data/pred_linear.csv data/pred_ridge.csv 2>/dev/null || true
git status
if git diff --cached --quiet; then
    echo "没有新文件要提交"
else
    git commit -m "$(cat <<'EOF'
作业2：多元线性回归与Ridge回归

C++手搓LinearRegression和RidgeRegression，加州房价数据。
EOF
)"
fi
echo "正在推送..."
git pull origin main --rebase 2>/dev/null || git pull origin master --rebase 2>/dev/null || true
git push -u origin main 2>/dev/null || git push -u origin master
echo "完成: $REMOTE"
